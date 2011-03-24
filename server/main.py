from sys import argv, exit
from random import randrange
import socket

from game import Game
from game_map import Map

def usage():
    print ("Usage: "
        "tot [map_size] [tanks_per_team] [shoot_range] [number_of_teams]")
    exit(-1)

def main():
    try:
        map_size = int(argv[1])
        tanks_per_team = int(argv[2])
        shoot_range = int(argv[3])
        number_of_teams = int(argv[4])
    except:
        usage()

    game_map = Map(map_size, shoot_range)
    game = Game(game_map, number_of_teams, tanks_per_team)

    while True:
        print Game.round()

if __name__ == "__main__":
    try:
        assert len(argv) == 5
    except:
        usage()

    main()

