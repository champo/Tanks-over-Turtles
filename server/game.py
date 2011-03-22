from random import randrange

from game_map import Map

class Game:
    def __init__(self, game_map, number_of_teams, tanks_per_team):
        self._map = game_map
        self._number_of_teams = number_of_teams
        self._tanks_per_team = tanks_per_team
        self._players = []

        self._initial_positions = []
        self._initialize()

    def _generate_random_positions(self, number):
        retval = []

        while len(retval) < number_of_positions:
            new_pair = (
                randrange(self._map.get_length()),
                randrange(self._map.get_length())
            )
            if new_pair not in retval:
                retval.append(new_pair)
        
        return retval

    def _initialize(self):
        sock = socket()
        sock.bind(('localhost', socket_port))
        sock.listen(5)
        number_of_clients = 0

        while number_of_clients < number_of_teams:
            new_socket = sock.accept()[0]
            pos = self._generate_random_positions(self._tanks_per_team)
            players.append(Player(
                new_socket,
                number_of_clients,
                pos,
                self._map
            ))
            number_of_clients += 1

    def round(self):

        # Reescribo el sigalert por un timeout de 1 segundo
        # Le pido a cada socket un mensaje
        # select() hasta que me hayan respondido todos
        # Ejecuto comandos en orden:
        #    - Move
        #    - Shoot
        #    - Laser
        # Mando respuestas
        # Mando END OF ROUND
        # Chequeo el estado del juego
