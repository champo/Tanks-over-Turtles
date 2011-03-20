
class Player:
    def __init__(self, socket, number, game):
        self._file = # Magia de champo para que socket sea un file
        self._number = number
        self._game = game

        self.send_initial_message()

    def send_initial_message(self):
        self._file.write("%d %d %d %d\n"%(
            self._map.get_size(),
            self._number,
            self._map.get_shoot_range(),
            self._number_of_tanks
        ))

    def query_next_mmessage(self):
        self._file.write("QUERY %d"%self._number)

    def send_result(self, message):
        self._file.write(message)

    def send_end_of_round(self):
        self._file.write("END OF ROUND.\n")

    def send_end_of_game(self):
        self._file.write("END OF GAME.\n")

