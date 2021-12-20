def pretty_print(grid):
    for row in grid:
        print(''.join(row))
    print("")

def pad_image(image, mode='.'):
    new_image = []
    new_image.append([mode] * (len(image[0]) + 2))
    for row in image:
        new_image.append([mode] + row + [mode])
    new_image.append([mode] * (len(image[0]) + 2))
    return new_image

def get_neighbours(i,j):
    for m in range(i-1,i+2):
        for n in range(j-1,j+2):
            yield m,n

def copy_image(image):
    new_image = []
    for row in image:
        new_image.append(row[:])
    return new_image

def advance(input_image, mode, step):
    if step % 2 == 1:
        input_image=pad_image(pad_image(pad_image(input_image)))
    else:
        input_image=pad_image(pad_image(pad_image(input_image, mode),mode),mode)
    new_image=copy_image(input_image)
    for i in range(len(input_image)):
        for j in range(len(input_image[i])):
                neighbours = [(m,n) for m,n in get_neighbours(i,j) if 0<=m<len(input_image) and 0<=n<len(input_image[i])]
                if len(neighbours)==9:
                    binary_number = ['0' if input_image[m][n]=='.' else '1' for m,n in neighbours]
                    int_number = int(''.join(binary_number),2)
                    new_image[i][j]=enhancement[int_number]
    if mode=='#':
        replace = '.' if step % 2 == 0 else '#'
        for i in range(len(new_image)):
            new_image[i][0]=replace
            new_image[i][len(new_image[0])-1]=replace
            new_image[0][i]=replace
            new_image[len(new_image[0])-1][i]=replace
    return new_image

def Print(image):
    for row in image:
        print("".join(row))

if __name__ == "__main__":
    lines_list = open('../data/20/data.txt').read().splitlines()
    enhancement = lines_list[0]
    input_image=[ [value for value in line] for line in lines_list[2:]]
    mode = enhancement[0]
    for step in range(2):
        input_image = advance(input_image, mode, step+1)
    Print(input_image)
    print("Part 1", sum(sum(1 for c in row if c=='#') for row in input_image))

    # input_image=[ [value for value in line] for line in lines_list[2:]]
    # mode = enhancement[0]
    # for step in range(50):
    #     input_image = advance(input_image, mode, step+1)
    #     if (step+1) % 10 == 0:
    #         print("Step:",step+1)
    # print("Part 2", sum(sum(1 for c in row if c=='#') for row in input_image))