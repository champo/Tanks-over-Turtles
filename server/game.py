from random import randrange

import signal
import time
import threading
from socket import socket
from random import randrange

from game_map import Map
from command import Command


class CallerThread(threading.Thread):
    def __init__(self, game, player, commands, list_lock):
        threading.Thread.__init__(self)
        self._game = game
        self._player = player
        self._commands = commands
        self._list_lock = list_lock

    def run(self):
        self._player.query_tanks()
        for command in self._player.get_commands():
            new_command = Command(
                self._game,
                self._player,
                command
            )
            self._list_lock.aquire()
            self._commands.append(new_command)
            self._list_lock.release()

class Game:
    def __init__(self, game_map, number_of_teams, tanks_per_team):
        self._map = game_map
        self._number_of_teams = number_of_teams
        self._tanks_per_team = tanks_per_team
        self._players = []
        self._active = {}
        self._socket_port = 1667

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

    def _active_players(self):
        return filter(lambda x: self._active[x], self._players)

    def _initialize(self):
        sock = socket()
        sock.bind(('localhost', self._socket_port))
        sock.listen(5)
        number_of_clients = 0

        while number_of_clients < self._number_of_teams:
            new_socket = sock.accept()[0]
            pos = self._generate_random_positions(self._tanks_per_team)
            player = Player(
                new_socket,
                number_of_clients,
                pos,
                self._map
            )
            self._players.append(player)
            self._active[player] = True
            number_of_clients += 1

    def _kill_players(self, to_be_killed):
        for player in to_be_killed:
            player.kill()        

    def round(self):

        commands = []

        # Trigger an alarm in two seconds if responses were not received
        def timedout():
            raise Exception("Timed out")
        oldsignal = signal.signal(signal.SIGALRM, timedout)
        signal.alarm(2)
        list_lock = threading.Lock()
        try:
            threads = []
            for player in self._active_players():
                thread = CallerThread(self, player, commands)
                thread.start()
                threads.append(thread)
            for thread in threads:
                thread.join()

        except:
            # You were too slow.
            # Let's be sure that we didn't interrupted somebody
            list_lock.aquire()
            for thread in threads:
                thread._Thread__stop()
                thread._Thread__delete()

            timed_out_players = [
                player for player in self._active_players()
                if not player in [
                    command.get_player() for command in commands
                ]
            ]
            self._kill_players(timed_out_players)

        finally:
            signal.alarm(0)
            signal.signal(signal.SIGALRM, oldsignal)

        for move in filter(lambda x: x.get_type() == 'MOVE', commands):
            self._map.move(move.get_tank(), move.get_direction())
            move.get_player().send_message('OK')

        for shoot in filter(lambda x: x.get_type() == 'SHOOT', commands):
            self._map.shoot(shoot.get_tank(), shoot.get_direction())
            shoot.get_player().send_message('OK')

        self._map.shoot_round_ended()

        for laser in filter(lambda x: x.get_type() == 'LASER', commands):
            move.get_player().send_response(
                self._map.laser(laser.get_tank(), laser.get_direction())
            )

        for player in self._active_players():
            player.send_end_of_round()

        if len(self._active_players()) < 2:
            for player in self._players:
                player.send_end_of_game()

