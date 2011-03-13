from map import Map

commands = {}

def play(map):
    """ Game busy-loop """
    while not is_over(map.get_alive_tanks()):
        commands = []
        for tank in map.get_alive_tanks():
            action, dir = tank.next_action()
            commands.append((tank, action, dir))
        commands.sort(lambda a,b: commands[a[1]] - commands[b[1]])

        for tank, action, dir in commands:
            commands[action][0](map, tank, dir)

        map.round_ended()

    map(lambda tank: tank.kill(), map.get_alive_tanks())

def action_with_response(action):
    def wrapped(map, tank, *args):
        tank.send_message(action(map, tank, *args))
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

commands = {
        "move": (Map.move, 0),
        "shoot": (Map.shoot, 1),
        "laser": (action_with_response(Map.laser), 2)
}

