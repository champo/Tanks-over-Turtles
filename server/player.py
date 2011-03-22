
class Player:
    def __init__(self, socket, number, pos, game_map):
        self._file = socket.makefile()
        self._number = number
        self._map = game_map
        self._tanks_pos = dict([(i, pos[i]) for i in xrange(len(pos))])
        self._tanks = ()

        self.send_initial_message()

    def send_initial_message(self):
        self._file.write("%d %d %d %d\n"%(
            self._map.get_size(),
            self._number,
            self._map.get_shoot_range(),
            self._number_of_tanks
        ))

    def query_tanks(self):
        tanks = ''.join(['%d'%tank.get_number() for tank in self._tanks])
        self._file.write("QUERY %s\n"%tanks)

    def get_commands(self):
        commands = ""
        while not 'END OF COMMANDS.\n' in commands:
            commands += self._file.read()
        return commands.split('\n')

    def send_result(self, message):
        self._file.write(message+'\n')

    def send_end_of_round(self):
        self._file.write("END OF ROUND.\n")

    def send_end_of_game(self):
        self._file.write("END OF GAME.\n")

