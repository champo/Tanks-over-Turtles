from socket import socket

if __name__ == '__main__':
    socket = socket()
    socket.connect('localhost', '5999')
    
    file = socket.makefile()

    TEAM_ID, SHOOT_RANGE = map(int, file.readline().split(' '))

    while True:
        direction = get_random_direction()

        file.write('LASER ' + get_random_direction() + '\n')
        TANK_ID, TANK_DIS = map(int, file.readline().split(' '))

        if TANK_ID and TANK_ID != TEAM_ID and TANK_DIS < SHOOT_RANGE:
            file.write('SHOOT ' + direction + '\n')

        file.write('MOVE ' + get_random_direction() + '\n')

