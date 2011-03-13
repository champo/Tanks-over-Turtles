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
    teams = []
    for team,command in enumerate(team_commands):
        tanks = [Tank(team + 1, command) for i in range(0, tanks_per_team)]

    play(map, teams)

if __name__ == "__main__":
    main()
