// Granblue Bingo.cpp : Defines the entry point for the console application.
//

#include <cstdlib>
#include <random>
#include <chrono>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

using namespace std;

unsigned seed = chrono::system_clock::now().time_since_epoch().count();
//random_device rd;
mt19937 prng(seed);
uniform_int_distribution<int> stuff(4, 6);
uniform_int_distribution<int> color(0, 4);
uniform_int_distribution<int> special(0, 24);

class sheet
{
public:
	sheet()
	{
		for (int i = 1; i < 26; ++i)
		{
			board_.push_back(i);
		}
		for (int i = 0; i < 5; ++i)
		{
		color_.push_back(i);
		}
		//inset shuffle logics for board_ and color_ here
		shuffle(board_.begin(), board_.end(), prng);
		shuffle(color_.begin(), color_.end(), prng);
		vector<int> tempcolor = color_;
		color_.clear();
		for (int i = 0; i < 5; ++i)
			for (int j = 0; j < 5; ++j)
			{
				color_.push_back(tempcolor[i]);
			}
		matches_.assign(25, false);
		color_matches_.assign(25, false);
		matches_[12] = true;
		color_matches_[12] = true;
		bingo_ = 0;
		super_ = 0;
		roll_num_ = 0;
	}
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
	void matchroll(int roll_num, int roll_col, char roll_type)
	{
		if (bingo_) return;
		int index = 0;
		while (1) 
		{	
			if (board_[index] == roll_num) { break; } 
			index++;
		}
		matches_[index] = true;
		if (color_[index] == roll_col) { color_matches_[index] = true; }
		if (roll_type != 'i' && index != 12 ) { ++roll_num_; }
		if (roll_num == 3)
		{
			int a;
			int b = 0;
			while (1)
			{
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

	bool checkbingo()
	{
		if (bingo_ == true) return false;
		//check horizontal
		for (int i = 0; i < 5; ++i)
		{
			int tempsum = 0;
			for (int j = 0; j < 5; ++j)
				tempsum += matches_[i * 5 + j];
			if (tempsum == 5)
			{
				bingo_ = true;
				return true;
			}
		}
		//check vertical
		for (int i = 0; i < 5; ++i)
		{
			int tempsum = 0;
			for (int j = 0; j < 5; ++j)
				tempsum += matches_[i + j * 5];
			if (tempsum == 5)
			{
				bingo_ = true;
				return true;
			}
		}
		//check other misc stuff
		int tempsum = matches_[0] + matches_[6] + matches_[12] + matches_[18] + matches_[24];
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
	bool checksuper()
	{
		if (super_ == true) return false;
		//check horizontal
		for (int i = 0; i < 5; ++i)
		{
			int tempsum = 0;
			for (int j = 0; j < 5; ++j)
				tempsum += color_matches_[i * 5 + j];
			if (tempsum == 5)
			{
				super_ = true;
				return true;
			}
		}
		//check vertical
		for (int i = 0; i < 5; ++i)
		{
			int tempsum = 0;
			for (int j = 0; j < 5; ++j)
				tempsum += color_matches_[i + j * 5];
			if (tempsum == 5)
			{
				super_ = true;
				return true;
			}
		}
		//check other misc stuff
		int tempsum = color_matches_[0] + color_matches_[6] + color_matches_[12] + color_matches_[18] + color_matches_[24];
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
	vector<int>board_;
	vector<bool>matches_;
	vector<bool>color_matches_;
	vector<int>color_;
	bool bingo_;
	bool super_;
	int roll_num_;
};

class game
{
public:
	game()
	{
		for (int i = 1; i < 26; ++i)
			masternumlist_.push_back(i);
		shuffle(masternumlist_.begin(), masternumlist_.end(), prng);
	}

	pair<int,int> bingo_roll()
	{
		int temp = masternumlist_.back();
		masternumlist_.pop_back();
		return pair<int,int>(temp, color(prng));
	}

private:
	vector<int> masternumlist_;
	int winners;
};

class player
{
public:
	player()
	{
		this->chips_ = 0;
		this->wins_ = 0;
		this->total_games_ = 0;
		this->super_bingos_ = 0;
		this->myboard_ = new sheet();
		for (int i = 0; i < 4; ++i) { placements_.push_back(0); }
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
	void add_chips(int a) { this->chips_ += a; }
	void add_win() { this->wins_ += 1; }
	void add_game() { this->total_games_ += 1; }
	void add_super() { this->super_bingos_ += 1; }
	void print_stats() { }
	void process_roll(int roll_num, int roll_col, char roll_type) { this->myboard_->matchroll(roll_num, roll_col, roll_type); }
	bool process_wins(int placement)
	{
		if (this->myboard_->checkbingo())
		{
			this->wins_++;
			if (this->myboard_->checksuper()) { this->super_bingos_++; }
			else if (placement == 1) {
				this->chips_ += 1000000; 
				placements_[0]++;
			}
			else if (placement == 2) {
				this->chips_ += 500000;
				placements_[1]++;
			}
			else if (placement == 3) {
				this->chips_ += 300000;
				placements_[2]++;
			}
			else if (placement == 4) {
				this->chips_ += 200000;
				placements_[3]++;
			}

			return true;
		}
		return false;
	}
	void print_player()
	{
		cout << "chips: " << this->chips_ << endl;
		cout << "wins: " << this->wins_ << endl;
		cout << "total games:" << this->total_games_ << endl;
		cout << "super bingos:" << this->super_bingos_ << endl;
		cout << "placements: " << this->placements_[0] << " " << this->placements_[1] << " " << this->placements_[2] << " " << this->placements_[3] << " " << endl;
	}
	void end_of_game() 
	{
		this->chips_ -= 100000;
		this->total_games_++;
		delete this->myboard_;
		this->myboard_ = new sheet();
	}

private:
	sheet* myboard_;
	int chips_;
	int wins_;
	int total_games_;
	int super_bingos_;
	vector<int> placements_;
};

int main()
{
	//assumptions.  add cin code here later
	int num_ppl = 20;
	int num_games = 5000;
	int placement = 1;
	bool placement_up = false;
	//build ppl
	vector<player*> players;
	for (int i = 0; i < num_ppl; ++i)
	{
		player* temp = new player();
		players.push_back(temp);
	}
	int games_played = 0;
	while (games_played < num_games)
	{
		game* current_game = new game();
		int first_rolls = stuff(prng);
		for (int i = 0; i < first_rolls; ++i)
		{
			pair<int,int> temp = current_game->bingo_roll();
			for (player* cur_player : players)
			{
				cur_player->process_roll(temp.first, temp.second, 'i');
			}
		}
		for (player* cur_player : players)
		{
			if (cur_player->process_wins(placement)) { placement_up = true; }
		}
		if (placement_up) { ++placement; }
		placement_up = false;
		for (int i = 0; i < 5; ++i)
		{
			pair<int, int> temp = current_game->bingo_roll();
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
		if (games_played % 100 == 0) cout << games_played << endl;
		games_played++;
	}
	int i = 0;
	for (player* cur_player : players)
	{
		cout << "player: " << i++ << endl;
		cur_player->print_player();
		cout << endl;
	}
}