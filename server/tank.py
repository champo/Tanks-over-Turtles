from subprocess import Popen, PIPE

class Tank(object):

    def __init__(self, team, command, range):
        self.__team = team
        self.__process = Popen(command.split(' '), stdin=PIPE, stdout=PIPE, stderr=PIPE)
        self.__alive = True

        self.send_response([team, range])

    def get_team(self):
        return self.__team

    def kill(self):
        """ OH NOES """
        self.__process.kill()
        self.__alive = False

    def is_alive(self):
        return self.__alive

    def next_action(self):
        """ Get a tuple with (action, direction) with the tanks next action """
        values = self.__process.stdin.readline().split(' ')[:2]
        return (values[0], values[1])

    def send_response(self, response):
        """ Write the response to the tank """

        if isinstance(response, 'str'):
            response = [response]

        self.__process.stdout.write(' '.join(response) + '\n')
        self.__process.stdout.flush()
