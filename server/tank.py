from subprocess import Popen
from socket import socket

class Tank(object):

    def __init__(self, team, command, range, socket_port):
        self.__team = team

        sock = socket()
        sock.bind(('localhost', socket_port))
        sock.listen(1)

        self.__process = Popen(command.strip().split(' ') + [str(socket_port)])
        self.__alive = True

        self.__socket = sock.accept()[0]
        sock.close()
        self.__file = self.__socket.makefile()
        self.send_response([team, range])

    def get_team(self):
        return self.__team

    def kill(self):
        """ OH NOES """
        self.__process.kill()
        self.__file.close()
        self.__socket.close()
        self.__alive = False

    def is_alive(self):
        return self.__alive

    def next_action(self):
        """ Get a tuple with (action, direction) with the tanks next action """
        if not self.__alive:
            return None
        values = self.__file.readline().strip().split(' ')
        return (values[0], values[1])

    def send_response(self, response):
        """ Write the response to the tank """

        if not self.__alive:
            return

        if isinstance(response, basestring):
            response = [response]

        self.__file.write(' '.join([str(value) for value in response]) + '\n')
        self.__file.flush()

