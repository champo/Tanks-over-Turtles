from sys import argv
from random import randrange
import socket

from game import play, action_with_response
from map import Map
from tank import Tank

def usage():
    print ("Usage: "
        "tot [map_size] [tanks_per_team] [shoot_range] [number_of_teams]")

def main():
    try:
        assert len(argv) == 5
    except:
        usage()

    try:
        map_size = int(argv[1])
        tanks_per_team = int(argv[2])
        shoot_range = int(argv[3])
        number_of_teams = int(argv[4])
    except:
        usage()

    map = Map(map_size, shoot_range)

    # This blocks until all clients are connected
    game = Game(map, number_of_teams, tanks_per_team)

    while True:
        print Game.round()

if __name__ == "__main__":
    main()

