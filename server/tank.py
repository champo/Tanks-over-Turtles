from subprocess import Popen, PIPE

class Tank(object):

    def __init__(self, team, command):
        self.__team = team
        # TODO: Start the inter process communication

    def get_team(self):
        return self.__team

    def kill(self):
        """ OH NOES """
        pass

    def next_action(self):
        """ Get a tuple with (action, direction) with the tanks next action """
        pass

    def send_response(self, response):
        """ Write the response to the tank """
        pass

