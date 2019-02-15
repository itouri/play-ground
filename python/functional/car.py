from random import random

def functional():
    time = 5
    car_positions = [1,1,1]

    while time:
        time -= 1
        print('')
        for i in range(len(car_positions)):
            if random() > 0.3:
                car_positions[i] += 1
            print('-' * car_positions[i])

def move_cars(probability, car_positions):
    # tmp = map(random() > property, car_positions)
    return map(lambda x: x + 1 if random() > probability else x, car_positions)

def output_car(car_position):
    return '-' * car_position

def draw(car_positions):
    #for i in range(len(car_positions)):
    #    print('-' * car_positions[i])
    print('')
    print('\n'.join(map(output_car,car_positions)))

def step(state):
    return {'time':state['time']-1, 'car_positions':move_cars(0.3, state['car_positions'])}

def race(state):
    draw(state['car_positions'])
    if state['time']:
        race(step(state))

if __name__ == '__main__':
    # functional()
    race({'time':5, 'car_positions':[1,1,1]})