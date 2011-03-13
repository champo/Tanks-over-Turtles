_ALTER_ROW = { 'UP': -1, 'DOWN': 1, 'LEFT': 0, 'RIGHT': 0 }
_ALTER_COL = { 'UP': 0, 'DOWN': 0, 'LEFT': -1, 'RIGHT': 1 }

class Map(object):

    def __init__(self, dimension, shoot_range):
        ''' Initialize the map '''
        self._tank_to_pos = {}
        self._dimension = dimension
        self._shoot_range = shoot_range
        self._to_be_killed = []
        self._rows = [{} for i in xrange(dimension)]
        self._cols = [{} for i in xrange(dimension)]

    def _valid_position(self, pos):
        ''' Returns True if 'pos' is a valid position in the map, and is
        in the correct form (a tuple or a list of only two elements)'''
        return ((type(pos) == type(()) or type(pos) == type([]))
            and (len(pos) == 2)
            and (0 <= pos[0] < self._dimension)
            and (0 <= pos[1] < self._dimension))

    def _move_pos_dir(self, pos, dir):
        ''' Return a new position, to the 'dir' of 'pos' '''
        return (
            pos[0] + _ALTER_ROW[dir] % self._dimension,
            pos[1] + _ALTER_COL[dir] % self._dimension
        )

    def _position_taken(self, pos):
        ''' Returns a boolean value, True iff the position is occupied '''
        return self._rows[pos[0]].has_key(pos[1])

    def _take_tank_out(self, tank):
        ''' Updates the inner data of this instance taking the tank out of
        its current position
        '''
        pos = self._tank_to_pos(tank)
        self._rows[pos[0]].pop(pos[1])
        self._cols[pos[1]].pop(pos[0])
    
    def _put_tank_in(self, tank, pos):
        ''' Puts a tank in a given position. If that position is already
        taken, will kill both tanks
        '''
        if (self._position_taken(pos)):
            tank.kill()
            self._get_tank_at(pos).kill()
            self._take_tank_out(tank)
            self._take_tank_out(self._get_tank_at(pos))

        else:
            self._tank_to_pos[tank] = pos
            self._rows[pos[0]][pos[1]] = tank
            self._cols[pos[1]][pos[0]] = tank

    def _get_tank_at(self, pos):
        ''' Returns the tank sitting in a given position '''
        return self._rows[pos[0]][pos[1]]

    def _get_next_thing(self, pos, dir):
        ''' Returns three elements in a list, with information on where is
        the next "interesting" thing in the map in the form
        (row, col, distance) '''
        line = []
        if dir == 'UP' or dir == 'DOWN':
            line = self._cols[pos[1]]
            me = pos[0]
        elif dir == 'LEFT' or dir == 'RIGHT':
            line = self._rows[pos[0]]
            me = pos[1]
        dir2 = 1 if dir in ['RIGHT', 'DOWN'] else -1
        dist = 1
        seek_pointer = me + dir2
        while dist < self._dimension and not line.has_key(seek_pointer):
            seek_pointer += dir2
            dist += 1

        if dir in ['UP', 'DOWN']:
            return (seek_pointer, pos[1], dist)
        elif dir in ['LEFT', 'RIGHT']:
            return (pos[0], seek_pointer, dist)
        return None

    def add_tank(self, tank, pos):
        ''' Adds a tank in the map in the position (row, col) given '''
        assert(tank.__class__.__name__ == 'Tank')
        assert(self._valid_position(pos))
        self._put_tank_in(tank, pos)

    def laser(self, tank, dir):
        ''' Checks for what is to the 'dir' of a given tank, and returns a\
        tuple with two integers: the first element is what it found, and the 
        second element is how far away it is '''
        assert(dir in _ALTER_COL.keys())
        assert(tank.__class__.__name__ == 'Tank')
        pos = self._tank_to_pos[tank]
        next_pos = self._get_next_thing(pos, dir)
        next_thing = self._rows[next_pos[0]][next_pos[1]]
        return (next_thing.get_team(), next_pos[2])

    def move(self, tank, dir):
        ''' Move a tank in a given direction '''
        assert(dir in _ALTER_COL.keys())
        assert(tank.__class__.__name__ == 'Tank')
        self._take_tank_out(tank)
        self._put_tank_in(tank, self._move_pos_dir
            (self._tank_to_pos[tank], dir))

    def shoot(self, tank, dir):
        ''' Makes a tank to shoot in a given direction '''
        assert(dir in _ALTER_COL.keys())
        assert(tank.__class__.__name__ == 'Tank')
        what = self._get_next_thing(self._tank_to_pos[tank], dir)
        if what[2] <= self._shoot_range:
            self._get_tank_at(what[:2]).kill()

    def get_alive_tanks(self):
        ''' Returns a list of alive tanks '''
        return self._tank_to_pos.keys()

    def round_ended(self):
        ''' This method should be called when the round is over, to 
        clean up all tanks that were killed in this round '''
        for tank in self._to_be_killed:
            self._take_tank_out(tank)
        self._to_be_killed = []

