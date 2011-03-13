from sys import argv
from socket import socket
from random import randrange

def get_random_direction():
    return ['UP', 'DOWN', 'LEFT', 'RIGHT'][randrange(4)]

if __name__ == '__main__':
    socket = socket()
    socket.connect(('localhost', int(argv[1])))

    data = socket.recv(1024)
    TEAM_ID, SHOOT_RANGE = map(int, data.split(' ')[:2])

    while True:
        direction = get_random_direction()

        socket.send('LASER ' + get_random_direction() + '\n')
        data = socket.recv(1024)
        TANK_ID, TANK_DIS = map(int, data.split(' ')[:2])

        if TANK_ID and TANK_ID != TEAM_ID and TANK_DIS < SHOOT_RANGE:
            socket.send('SHOOT ' + direction + '\n')

        socket.send('MOVE ' + get_random_direction() + '\n')

