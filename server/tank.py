from subprocess import Popen
from socket import socket

class Tank(object):

    def __init__(self, team, command, range):
        self.__team = team

        self.__socket = socket()
        self.__socket.bind(('localhost', 5999))
        self.__socket.listen(1)

        self.__process = Popen(command.split('\n')[0].split(' '))
        self.__alive = True

        self.__socket.accept()
        self.__file = self.__socket.makefile()
        self.send_response([team, range])

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
        if not self.__value:
            return None
        values = self.__file.readline().split(' ')[:2]
        return (values[0], values[1])

    def send_response(self, response):
        """ Write the response to the tank """

        if not self.__alive:
            return

        if isinstance(response, basestring):
            response = [response]

        self.__file.write(' '.join([str(value) for value in response]) + '\n')
