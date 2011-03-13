from map import Map

command_data = {}

def play(map):
    """ Game busy-loop """
    while not is_over(map.get_alive_tanks()):
        commands = []
        for tank in map.get_alive_tanks():
            action, dir = tank.next_action()
            commands.append((tank, action, dir))
        commands.sort(lambda a,b: command_data[a[1]][1] - command_data[b[1]][1])

        for tank, action, dir in commands:
            command_data[action][0](map, tank, dir)

        map.round_ended()

    [tank.kill() for tank in map.get_alive_tanks()]

def action_with_response(action):
    def wrapped(map, tank, *args):
        tank.send_response(action(map, tank, *args))
    return wrapped

def is_over(tanks):
    """ Check whether the game is over """
    if len(tanks) == 0:
        return True

    print [tank.get_team() for tank in tanks]

    team = tanks[0].get_team()
    for tank in tanks[1:]:
        if tank.get_team() != team:
            return False

    return True

command_data = {
        "MOVE": (Map.move, 0),
        "SHOOT": (Map.shoot, 1),
        "LASER": (action_with_response(Map.laser), 2)
}

