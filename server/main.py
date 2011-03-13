from sys import argv

from game import play

def main():
    assert len(argv) == 5
    map_size = int(argv[1])
    tanks_per_team = int(argv[2])
    team_file_name = argv[3]
    shoot_range = int(argv[4])
    with open(team_file_name) as f:
        teams_commands = f.readlines()

    map = Map(map_size, shoot_range)
    for team,command in enumerate(team_commands):
        for i in range(0, tanks_per_team):
            map.add_tank(Tank(team + 1, command, shoot_range))

    play(map)

if __name__ == "__main__":
    main()
