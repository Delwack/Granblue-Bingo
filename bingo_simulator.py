# Bingo simulator: specifications
# Bingo is a 25 number sheet (1-25).  each column also has a distinct color
# Center square in the bingo sheet automatically matches number and color, but the number there can still be drawn
# balls drawn have both a number and a color
# start the game off by drawing a random number of balls; 4-6 all at once
# then draw 5  balls one at a time.
# There is also a 'special ball'
# the special ball is individually rolled for each player from the remaining squares.  This number can be drawn again
# from the master draw
# the special ball is drawn after 3 matches are made (a match is not made if the center square is drawn because it
# is already matched on the board.)
# Prizes are graduated from 1st place (10x) 2nd place (5x) 3rd place (3x) 4th place (2x).
# ties are all considered the same place. Multiple ppl getting 1st does not cause other places to be skipped
# Super Bingo:  If you match both color and number for your bingo, you gain 77,777,777 instead.
# colors are: Orange, Red, Green, Purple, Blue

# import random library
import random
import time

random.seed(time.clock())

class Sheet:
    # board is top to bottom, left to right.
    board = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25]
    matches = [0] * 25
    colormatches = [0] * 25
    # colors are left to right.  entire column is one color
    color = ["red", "green", "blue", "orange", "purple"]
    bingo = False
    super_bingo = False
    roll_num = 0
    already_won = False

    def __init__(self):
        random.shuffle(self.board)
        for garbage in self.color:
            self.color.pop
        self.color.append("red")
        self.color.append("green")
        self.color.append("blue")
        self.color.append("orange")
        self.color.append("purple")
        random.shuffle(self.color)
        self.color = [self.color[0]]*5 + [self.color[1]]*5 + [self.color[2]]*5 + [self.color[3]]*5 + [self.color[4]]*5
        self.matches = [0] * 25
        self.colormatches = [0] * 25
        self.matches[12] = 1
        self.colormatches[12] = 1
        already_won = False
        super_bingo = False
        bingo = False

    def initroll(self, number, color):
        self.matches[self.board.index(number)] = 1
        if self.color[self.board.index(number)] == color: self.colormatches[self.board.index(number)] = 1
        # self.printroll(number, color)

    def matchroll(self, number, color):
        if self.bingo:
            return
         #   print "bingo detected, returning"
        self.matches[self.board.index(number)] = 1
        if self.color[self.board.index(number)] == color: self.colormatches[self.board.index(number)] = 1
        if self.color[self.board.index(number)] == 12: self.roll_num -= 1
        self.roll_num += 1
        # self.printroll(number, color)

    def matchspecial(self):
        if self.bingo:
            return
         #   print "bingo detected, returning"
        elif self.roll_num == 3:
            self.roll_num += 1
            temp = False
            while not temp:
                temp2 = self.board.index(random.choice(self.board))
                if not self.matches[temp2]:
                    temp = True
                    self.matches[temp2] = True
                    self.colormatches[temp2] = True
                 #   print "Special roll!"
                 #   self.printroll(temp2, self.color[temp2])


    def checkbingo(self):
        if self.already_won : return False
        #check vertical
        elif sum(self.matches[0:5]) == 5 or sum(self.matches[5:10]) == 5 or sum(self.matches[10:15]) == 5 or sum(self.matches[15:20]) == 5 or sum(self.matches[20:25]) == 5:
            self.bingo = True
            self.already_won = True
         #   print "Bingo!"
            return True
        #check horizontal
        elif sum(self.matches[0:25:5]) == 5 or sum(self.matches[1:25:5]) == 5 or sum(self.matches[2:25:5]) == 5 or sum(self.matches[3:25:5]) == 5 or sum(self.matches[4:25:5]) == 5:
            self.bingo = True
            self.already_won = True
         #   print "Bingo!"
            return True
        #check diagonal
        elif sum(self.matches[0:25:6]) == 5 or sum(self.matches[4:21:4]) == 5:
            self.bingo = True
            self.already_won = True
         #   print "Bingo!"
            return True
        return False

    def checksuper(self):
        #check vertical
        if sum(self.colormatches[0:5]) == 5 or sum(self.colormatches[5:10]) == 5 or sum(self.colormatches[10:15]) == 5 or sum(self.colormatches[15:20]) == 5 or sum(self.colormatches[20:25]) == 5:
            self.super_bingo = True
            return True
        # check horizontal
        elif sum(self.colormatches[0:25:5]) == 5 or sum(self.colormatches[1:25:5]) == 5 or sum(self.colormatches[2:25:5]) == 5 or sum(self.colormatches[3:25:5]) == 5 or sum(self.colormatches[4:25:5]) == 5:
            self.super_bingo = True
            return True
        # check diagonal
        elif sum(self.colormatches[0:25:6]) == 5 or sum(self.colormatches[4:21:4]) == 5:
            self.super_bingo = True
            return True
        return False

    def reset(self):
        self.board = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25]
        self.matches = [0] * 25
        self.colormatches = [0] * 25
        self.color = ["red", "green", "blue", "orange", "purple"]
        self.bingo = False
        self.super_bingo = False
        self.roll_num = 0
        self.already_won = False

    def outputsheet(self):
        print "The board:"
        print self.board

    def printroll(self, number, color):
        print "Number: %d color %s" % (number, color)


class Game:
    players = 0
    gameboards = []
    masternumlist = []
    winners = []
    place_count = 1
    color = ["red", "green", "blue", "orange", "purple"]

    def __init__(self, playernum):
        self.players = playernum
        count = 0
        # setup number of players
        for oldsheet in self.gameboards:
            self.gameboards.pop()
        for stuff in self.masternumlist:
            self.masternumlist.pop()
        temp = 1
        place_count = 1
        while  temp < 26:
            self.masternumlist.append(temp)
            temp +=1

        while count < self.players:
            self.gameboards.append(Sheet())
            count += 1

    def startgame(self):
        # do first rolls
        new_winer = False
        first_rolls = random.sample(self.masternumlist, random.choice([4,5,6]))
        # print "Init number of rolls are: %d" % len(first_rolls)
        for roll in first_rolls:
            self.masternumlist.remove(roll)
            tempcolor = random.choice(self.color)
            for sheets in self.gameboards:
                sheets.initroll(roll, tempcolor)

    def check_winners(self):
        for sheets in self.gameboards:
            if sheets.checkbingo():
                self.winners.append(self.gameboards.index(sheets))
        return self.winners

    def clear_winners(self):
        if len(self.winners): self.place_count += 1
        for deletewins in self.winners:
            self.winners.pop()

    def bingoroll(self):
        roll = random.choice(self.masternumlist)
        self.masternumlist.remove(roll)
        tempcolor = random.choice(self.color)
        for sheets in self.gameboards:
            sheets.matchroll(roll, tempcolor)

    def specialroll(self):
        for sheets in self.gameboards:
            sheets.specialroll()

    def reset(self):
        self.players = 0
        self.gameboards = []
        self.masternumlist = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25]
        self.winners = []
        self.place_count = 1
        self.color = ["red", "green", "blue", "orange", "purple"]

class Player:
    chips = 0
    wins = 0
    totalgames = 0
    wstreak = 0
    lstreak = 0
    places = []
    sbingo = 0

    def __init__(self):
        chips = 0

    def printstats(self):
        print self.chips
        print self.wins
        print self.totalgames
        print self.sbingo
        print self.places

payout = {
    1 : 100000,
    2 : 50000,
    3 : 30000,
    4 : 20000
}
num_players = 1
num_games = 100000
players = []
temp = 0
# build player db
while temp < num_players:
    players.append(Player())
    temp += 1
temp = 0
# run total number of games
while temp < num_games:
    for player in players:
        player.totalgames += 1
        player.chips -= 10000
    if temp%1000 == 0: print "playing game #%d" % temp
    game = Game(num_players)
    # game.gameboards[0].outputsheet()
    game.startgame()
    if len(game.check_winners()):
        for winner in game.winners:
            players[winner].wins += 1
            if game.gameboards[winner].checksuper():
                players[winner].sbingo += 1
                players[winner].chips += 7777777
            else: players[winner].chips += payout[game.place_count]
        game.place_count += 1
        game.clear_winners()
    temp2 = 0
    while temp2 < 5:
        temp2 += 1
        game.bingoroll()
        if len(game.check_winners()):
            for winner in game.winners:
                players[winner].wins += 1
                if game.gameboards[winner].checksuper():
                    players[winner].sbingo += 1
                    players[winner].chips += 7777777
                else:
                    players[winner].chips += payout[game.place_count]
            game.place_count += 1
            game.clear_winners()
        if temp2 == 2 or temp2 == 3:
            for sheet in game.gameboards:
                sheet.matchspecial()
    temp += 1

players[0].printstats()