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

        shots_round_ended_called = False
        for tank, action, dir in commands:
            if action == 'LASER' and not shots_round_ended_called:
                map.round_ended()
                shots_round_ended_called = True
            if tank.is_alive():
                command_data[action][0](map, tank, dir)

    if map.get_alive_tanks():
        print("Winning team: " + str(map.get_alive_tanks()[0].get_team()))
    else:
        print("Draw.")
    [tank.kill() for tank in map.get_alive_tanks()]

def action_with_response(action):
    def wrapped(map, tank, *args):
        tank.send_response(action(map, tank, *args))
    return wrapped

def is_over(tanks):
    """ Check whether the game is over """
    if len(tanks) == 0:
        return True

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

