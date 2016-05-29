// Granblue Bingo.cpp : Defines the entry point for the console application.
//

/*
  Granblue Bingo simulation

  Game Rules:
  5x5 bingo board, numbers 1-25.  each square has a number chosen for it at random.
  each column also has a color associated with it.
  each ball drawn has both a color and a number.
  center square is always free, but the number under it can still be drawn by any ball except the bonus ball.
  game starts by drawing 4-6 (number randomly chosen) balls.
  then, 5 more balls are drawn.
  you also get 1 bonus ball after 3 balls that you can place on your board are drawn.  If you draw the center number the bonus ball is on the 4th instead.
  the bonus ball is individualized to each person, will only hit a square you do not have,
  and can be drawn again from the master draws.  The special ball always matches square color too.
  if you match any row, column or one of the two diagnols, you get bingo.
  If you match the colors in addition to the numbers, you get super bingo.
  rooms contain 5, 10 or 20 players.
  Payout is dependant on when you bingo within your room.  
  payouts are 10x 1st, 5x 2nd, 3x 3rd, 2x 4th, 0 5th+
  all bingos on the same turn are considered the same place.  even if multiple people get 1st 
  on the same turn, the next person (or set of people to bingo) will get 2nd.

  the ultimate goal of this program is to calculate average returns, and get some sense of odds.

  Please let me know if you find any errors.  The code and models need more extensive testing.
*/
#include <cstdlib>
#include <random>
#include <chrono>
#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>

using namespace std;

unsigned seed = chrono::system_clock::now().time_since_epoch().count();
mt19937 prng(seed);
uniform_int_distribution<long> stuff(4, 6);
uniform_int_distribution<long> color(0, 4);
uniform_int_distribution<long> special(0, 24);

class sheet
{
public:
	sheet()
	{
		//build your board with #'s 1-25, board runs top to bottom left to right.
		for (long i = 1; i < 26; ++i)
		{
			board_.push_back(i);
		}
		for (long i = 0; i < 5; ++i)
		{
			color_.push_back(i);
		}
		//shuffle both color and numbers
		shuffle(board_.begin(), board_.end(), prng);
		shuffle(color_.begin(), color_.end(), prng);
		vector<long> tempcolor = color_;
		color_.clear();
		//duplicate the colors down each row
		for (long i = 0; i < 5; ++i)
			for (long j = 0; j < 5; ++j)
			{
				color_.push_back(tempcolor[i]);
			}
		//build vectors to store matching data
		matches_.assign(25, false);
		color_matches_.assign(25, false);
		matches_[12] = true;
		color_matches_[12] = true;
		bingo_ = 0;
		super_ = 0;
		roll_num_ = 0;
	}
	//copy constructor
	void operator=(const sheet &S)
	{
		board_ = S.board_;
		matches_ = S.matches_;
		color_matches_ = S.color_matches_;
		color_ = S.color_;
		bingo_ = S.bingo_;
		super_ = S.super_;
		roll_num_ = S.roll_num_;
	}
	/*sheet(const sheet &S)
	{
		board_ = S.board_;
		matches_ = S.matches_;
		color_matches_ = S.color_matches_;
		color_ = S.color_;
		bingo = S.bingo;
		super = S.super;
		roll_num = S.roll_num;
	}*/
	//this performs all the matching logic.  feed it a ball, it'll match on the sheet.
	void matchroll(long roll_num, long roll_col, char roll_type)
	{
		if (bingo_) return;
		long index = 0;
		//find where in my grid the number is
		while (1) 
		{	
			if (board_[index] == roll_num) { break; } 
			index++;
		}
		//I've matched
		matches_[index] = true;
		//determine if color matches too
		if (color_[index] == roll_col) { color_matches_[index] = true; }
		//if it's the initial rolls or the center ball you rolled you don't get a step to the bonus ball
		if (roll_type == 'r' && !(index == 12)) { ++roll_num_; }
		//once you've achieved the bonus ball...
		if (roll_num == 3)
		{
			roll_num_++;
			long a;
			long b = 0;
			while (1)
			{
				//find a free ball
				 a = special(prng);
				 if (!matches_[a])
				 {
					 matches_[a] = true;
					 color_matches_[a] = true;
					 break;
				 }
			}
		}
	}

	//this checks for bingos.
	bool checkbingo()
	{
		if (bingo_ == true) return false;
		//check horizontal
		for (long i = 0; i < 5; ++i)
		{
			long tempsum = 0;
			for (long j = 0; j < 5; ++j)
				tempsum += matches_[i * 5 + j];
			if (tempsum == 5)
			{
				bingo_ = true;
				return true;
			}
		}
		//check vertical
		for (long i = 0; i < 5; ++i)
		{
			long tempsum = 0;
			for (long j = 0; j < 5; ++j)
				tempsum += matches_[i + j * 5];
			if (tempsum == 5)
			{
				bingo_ = true;
				return true;
			}
		}
		//check diags
		long tempsum = matches_[0] + matches_[6] + matches_[12] + matches_[18] + matches_[24];
		if (tempsum == 5)
		{
			bingo_ = true;
			return true;
		}
		tempsum = matches_[4] + matches_[8] + matches_[12] + matches_[16] + matches_[20];
		if (tempsum == 5)
		{
			bingo_ = true;
			return true;
		}
		return false;
	}

	//this checks for super bingos.
	bool checksuper()
	{
		if (super_ == true) return false;
		//check horizontal
		for (long i = 0; i < 5; ++i)
		{
			long tempsum = 0;
			for (long j = 0; j < 5; ++j)
				tempsum += color_matches_[i * 5 + j];
			if (tempsum == 5)
			{
				super_ = true;
				return true;
			}
		}
		//check vertical
		for (long i = 0; i < 5; ++i)
		{
			long tempsum = 0;
			for (long j = 0; j < 5; ++j)
				tempsum += color_matches_[i + j * 5];
			if (tempsum == 5)
			{
				super_ = true;
				return true;
			}
		}
		//check diags
		long tempsum = color_matches_[0] + color_matches_[6] + color_matches_[12] + color_matches_[18] + color_matches_[24];
		if (tempsum == 5)
		{
			super_ = true;
			return true;
		}
		tempsum = color_matches_[4] + color_matches_[8] + color_matches_[12] + color_matches_[16] + color_matches_[20];
		if (tempsum == 5)
		{
			super_ = true;
			return true;
		}
		return false;
	}
	bool bingo() { return bingo_; }

private:
	vector<long>board_;
	vector<bool>matches_;
	vector<bool>color_matches_;
	vector<long>color_;
	bool bingo_;
	bool super_;
	long roll_num_;
};

//this controls the list of balls to be drawn
class game
{
public:
	game()
	{
		//build the number list and shuffle it
		for (long i = 1; i < 26; ++i)
			masternumlist_.push_back(i);
		shuffle(masternumlist_.begin(), masternumlist_.end(), prng);
	}

	//draw a ball.
	pair<long,long> bingo_roll()
	{
		long temp = masternumlist_.back();
		masternumlist_.pop_back();
		return pair<long,long>(temp, color(prng));
	}

private:
	vector<long> masternumlist_;
};

//this class represents individual players and keeps track of performance.  Sheets (bingo boards) are owned by players.
class player
{
public:
	player(int bet)
	{
		this->chips_ = 0;
		this->wins_ = 0;
		this->total_games_ = 0;
		this->super_bingos_ = 0;
		this->myboard_ = new sheet();
		this->bet_ = bet;
		for (long i = 0; i < 4; ++i) { placements_.push_back(0); }
	}
	~player()
	{
		delete myboard_;
	}
	void operator=(const player &P)
	{
		this->chips_ = P.chips_;
		this->wins_ = P.wins_;
		this->total_games_ = P.total_games_;
		this->super_bingos_ = P.super_bingos_;
		this->myboard_ = P.myboard_;
	}
	//this passes on the process roll command to the board
	void process_roll(long roll_num, long roll_col, char roll_type) { this->myboard_->matchroll(roll_num, roll_col, roll_type); }
	//this looks for and processes wins
	bool process_wins(long placement)
	{
		if (this->myboard_->checkbingo())
		{
			this->wins_++;
			if (this->myboard_->checksuper()) { this->super_bingos_++; }
			else if (placement == 1) {
				this->chips_ += bet_ * 10; 
				placements_[0]++;
			}
			else if (placement == 2) {
				this->chips_ += bet_ * 5;
				placements_[1]++;
			}
			else if (placement == 3) {
				this->chips_ += bet_ * 3;
				placements_[2]++;
			}
			else if (placement == 4) {
				this->chips_ += bet_ * 2;
				placements_[3]++;
			}

			return true;
		}
		return false;
	}
	//this outputs statistics
	void output_player(ofstream& F)
	{
		F << "chips (no sb)" << '\t' << "wins" << '\t' << "total games" << '\t' << "super bingos" << '\t' << "1st" << '\t' << "2nd" << '\t' << "3rd" << '\t' << "4th" << '\t' << "chips inc s.bingo" << endl;
		F << chips_ << '\t' << wins_ << '\t' << total_games_ << '\t' << super_bingos_ << '\t' << placements_[0] << '\t' << placements_[1] << '\t' << placements_[2] << '\t' << placements_[3] << '\t' << chips_+super_bingos_*bet_*777 << endl;
	}
	void print_player()
	{
		cout << "chips: " << this->chips_ << endl;
		cout << "wins: " << this->wins_ << endl;
		cout << "total games:" << this->total_games_ << endl;
		cout << "super bingos:" << this->super_bingos_ << endl;
		cout << "placements: " << this->placements_[0] << " " << this->placements_[1] << " " << this->placements_[2] << " " << this->placements_[3] << " " << endl;
	}
	//this function concludes the game, subtracting your initial bet and giving you a new bingo sheet for the next game.
	void end_of_game() 
	{
		this->chips_ -= 10;
		this->total_games_++;
		delete this->myboard_;
		this->myboard_ = new sheet();
	}
	long chips() { return this->chips_; }
	long wins() { return this->wins_; }
	long super_bingos() { return this->super_bingos_; }
	const vector<long>& placements() { return this->placements_;  }

private:
	sheet* myboard_;
	long chips_;
	long wins_;
	long total_games_;
	long super_bingos_;
	vector<long> placements_;
	long bet_;
};

long main()
{

	ofstream F;
	long num_ppl = 20;
	long num_games = 1000000;
	int bet = 0;
	F.open("Granblue Bingo Simulation.txt", ofstream::out | ofstream::trunc);
	do
	{
		cout << "how many people at a table?" << endl;
		cin >> num_ppl;
	} while (num_ppl <= 0);

	do
	{
		cout << "how many games to run?" << endl;
		cin >> num_games;
	} while (num_games <= 0);
	if (num_ppl <= 5) bet = 1;
	else if (num_ppl <= 10) bet = 10;
	else bet = 100;
	
	//placement keeps track of where we are in terms of 1st/2nd/3rd/4th place in the bingo game.
	long placement = 1;
	bool placement_up = false;
	//build ppl
	vector<player*> players;
	for (long i = 0; i < num_ppl; ++i)
	{
		player* temp = new player(bet);
		players.push_back(temp);
	}
	//play games
	long games_played = 0;
	while (games_played < num_games)
	{
		game* current_game = new game();
		//this is the first initial rolls
		long first_rolls = stuff(prng);
		for (long i = 0; i < first_rolls; ++i)
		{
			pair<long,long> temp = current_game->bingo_roll();
			for (player* cur_player : players)
			{
				cur_player->process_roll(temp.first, temp.second, 'i');
			}
		}
		//check if anyone won
		for (player* cur_player : players)
		{
			if (cur_player->process_wins(placement)) { placement_up = true; }
		}
		if (placement_up) { ++placement; }
		placement_up = false;
		//these are the next set of regular rolls
		for (long i = 0; i < 5; ++i)
		{
			pair<long, long> temp = current_game->bingo_roll();
			for (player* cur_player : players)
			{
				cur_player->process_roll(temp.first, temp.second, 'r');
				if (cur_player->process_wins(placement)) { placement_up = true; }
			}
			if (placement_up) { ++placement; }
			placement_up = false;
		}
		placement = 1;
		for (player* cur_player : players)
		{
			cur_player->end_of_game();
		}
		delete current_game;
		if (games_played % 1000 == 0) cout << games_played << endl;
		games_played++;
	}
	long i = 0;
	long tot_chips = 0;
	long tot_sup_bing = 0;
	long wins = 0 ;
	vector<long> placements;
	placements.assign(4, 0);
	for (player* cur_player : players)
	{
		F << "player: " << i++ << endl;
		cur_player->output_player(F);
		F << endl;
		tot_chips += cur_player->chips();
		wins += cur_player->wins();
		tot_sup_bing += cur_player->super_bingos();
		for (int i = 0; i < 4;i++)
		{
			placements[i] += cur_player->placements()[i];
		}
	}
	F << endl << endl << "tot chips: " << tot_chips << endl << "wins: " << wins << endl << "Total super bingos: " << tot_sup_bing << endl;
	for (int i = 0; i < 4; i++)
	{
		F << "Place " << i << ": " << placements[i] << endl;
	}
	cout << endl << endl << "tot chips: " << tot_chips << endl << "wins: " << wins << endl << "Total super bingos: " << tot_sup_bing << endl;
	for (int i = 0; i < 4; i++)
	{
		cout << "Place " << i << ": " << placements[i] << endl;
	}
}