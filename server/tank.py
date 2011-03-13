from subprocess import Popen
from socket import socket

class Tank(object):

    socket_port = 5999

    def __init__(self, team, command, range):
        self.__team = team

        sock = socket()
        Tank.socket_port += 1
        sock.bind(('localhost', Tank.socket_port))
        sock.listen(1)

        self.__process = Popen(command.strip().split(' ') + [str(Tank.socket_port)])
        self.__alive = True

        self.__socket = sock.accept()[0]
        sock.close()
        self.send_response([team, range])
        self._buffer = ''

    def get_team(self):
        return self.__team

    def kill(self):
        """ OH NOES """
        self.__process.kill()
        self.__socket.close()
        self.__alive = False

    def is_alive(self):
        return self.__alive

    def next_action(self):
        """ Get a tuple with (action, direction) with the tanks next action """
        if not self.__alive:
            return None
        self._buffer += self.__socket.recv(1024)
        index = self._buffer.find('\n')
        values = self._buffer[:index]
        self._buffer = self._buffer[index+1:]
        values = values.strip().split(' ')
        return (values[0], values[1])

    def send_response(self, response):
        """ Write the response to the tank """

        if not self.__alive:
            return

        if isinstance(response, basestring):
            response = [response]

        self.__socket.send(' '.join([str(value) for value in response]) + '\n')
