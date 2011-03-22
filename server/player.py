
class Player:
    def __init__(self, socket, number, pos, game_map):
        self._file = socket.makefile()
        self._number = number
        self._map = game_map
        self._tanks = dict([(i, pos[i]) for i in xrange(len(pos))])

        self.send_initial_message()

    def send_initial_message(self):
        self._file.write("%d %d %d %d\n"%(
            self._map.get_size(),
            self._number,
            self._map.get_shoot_range(),
            self._number_of_tanks
        ))

    def query_next_mmessage(self, tank):
        self._file.write("QUERY %d\n"%tank)

    def send_result(self, message):
        self._file.write(message)

    def send_end_of_round(self):
        self._file.write("END OF ROUND.\n")

    def send_end_of_game(self):
        self._file.write("END OF GAME.\n")

