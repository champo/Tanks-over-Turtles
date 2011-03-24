import re

class Command:
    ''' All this class does is to parse a command and provide easy
    ways to access it '''
    def __init__(self, game, player, command):
        self._player = player
        tank_number, action_type, direction = \
            re.match('^(\d+) ([^ ]+) ([^ \n$]+)', command).groups()
        self._tank = player.get_tank(int(tank_number))
        self._type = action_type
        self._direction = direction

    def get_player(self):
        ''' The player that raised the command '''
        return self._player

    def get_tank(self):
        ''' The tank the command reffers to '''
        return self._tank

    def get_direction(self):
        ''' The direction of the command '''
        return self._direction

    def get_type(self):
        ''' The type of the command (MOVE, SHOOT, LASER) '''
        return self._type

