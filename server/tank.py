from subprocess import Popen
from socket import socket

class Tank(object):
    socket_port = 6999

    def __init__(self, team, command, range):
        self.__team = team

        self.__socket = socket()
        Tank.socket_port += 1
        self.__socket.bind(('localhost', Tank.socket_port))
        self.__socket.listen(1)

        self.__process = Popen(command.split('\n')[0].split(' ') + [str(Tank.socket_port)])
        self.__alive = True

        self.__socket.accept()
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
        if not self.__alive:
            return None
        values = None
        while not values:
            values = self.__socket.recv(1024)
        
        values = valus.split(' ')[:2]
        return (values[0], values[1])

    def send_response(self, response):
        """ Write the response to the tank """

        if not self.__alive:
            return

        if isinstance(response, basestring):
            response = [response]

        self.__socket.send(' '.join([str(value) for value in response]) + '\n')
