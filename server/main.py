from sys import argv
from random import randrange
import socket

from game import play, action_with_response
from map import Map
from tank import Tank

def main():
    assert len(argv) == 5
    map_size = int(argv[1])
    tanks_per_team = int(argv[2])
    team_file_name = argv[3]
    shoot_range = int(argv[4])
    # team_commands = None
    with open(team_file_name) as f:
        team_commands = f.readlines()

    map = Map(map_size, shoot_range)
    positions = []
    while len(positions) != tanks_per_team * len(team_commands):
        new_pos = (randrange(map_size), randrange(map_size))
        if new_pos not in positions:
            positions.append(new_pos)
    for team,command in enumerate(team_commands):
        for i in range(0, tanks_per_team):
            success = False
            socket_port = 5999
            while not success:
                try:
                    map.add_tank(Tank(
                            team + 1, command, shoot_range, socket_port
                        ), positions.pop()
                    )
                except socket.error as e:
                    socket_port += 1
                else:
                    success = True

    try:
        play(map)
    except Exception as e:
        for tank in map.get_alive_tanks():
            tank.kill()
        print("An error has occurred.")
        raise e

if __name__ == "__main__":
    main()
