#!/usr/bin/env python3
from __future__ import print_function
import sys
from collections import Counter


# YOUR CODE GOES HERE
class StreetMap:
    def __init__(self):
        """
        Initialize variables for whole class
        """
        self.streets = dict()
        self.old_v = dict()
        self.start_idx = 1

    def __init_vars__(self): 
        """
        Initialize all needed variables when calling 'gg'
        """
        self.v = dict()  # storing vertexes
        self.e = []  # storing edges
        self.lines = []  # storing all line segments
        self.line_segmented = []  # storing all straight lines

    def __get_lines__(self, key):
        # Get all line segments of a specific street
        lines = []
        segments = self.streets[key]
        for i in range(len(segments) - 1):
            lines.append([segments[i], segments[i + 1]])
        return lines

    def __add_lines__(self, key):
        # Add a street's line segments into self.lines
        self.lines += self.__get_lines__(key)

    def __check_two_lines_overlapped(self, new_line):

        def two_line_overlapped(line1, line2):
            # Check if two lines are same straight line
            counter = 0
            for point in line1:
                if point in line2:
                    counter += 1
            if counter >= 2:  # if two lines have more than two share points, they are on a same line
                return True
            return False

        def merge_two_lines(line1, line2):
            # Merge two lines into a new line
            return sorted(list(set(line2).union(set(line1))))

        for i in range(len(self.line_segmented)):
            if two_line_overlapped(new_line, self.line_segmented[i]):
                self.line_segmented[i] = merge_two_lines(new_line, self.line_segmented[i])
                return

        self.line_segmented.append(new_line)

    def __vertex_mapping__(self, new_vertexes):
        for vertex in new_vertexes:
            if vertex not in self.v:
                if vertex not in self.old_v:
                    self.v[vertex] = self.start_idx
                    self.start_idx += 1
                else:
                    self.v[vertex] = self.old_v[vertex]

    def __generate_output__(self):
        # print('V = {')
        # for key, item in sorted(self.v.items(), key=lambda x: x[1]):
        #     print(
        #         "  " + "{0:<4}".format(str(self.v[key]) + ':') + "({0:.2f},{1:.2f})".format(key[0], key[1]))
        # print('}')
        # print('E = {')
        # s = ',\n'
        # e_str = s.join(['  <%i,%i>' % (edge[0], edge[1]) for edge in self.e])
        # if len(self.e) > 0:
        #     print(e_str)
        # print('}')
        print('V {0}'.format(self.start_idx - 1))
        s = ','
        e_str = s.join(['<%i,%i>' % (edge[0], edge[1]) for edge in self.e])
        if len(self.e) >= 0:
            print('E {' + e_str + '}')
        sys.stdout.flush()


    def add_street(self, street_name, segments):
        if street_name in self.streets:
            raise Exception('This street already exists.')
        else:
            self.streets[street_name] = segments

    def remove_street(self, street_name):
        if street_name not in self.streets:
            raise Exception('`rm` specified for a street does not exist.')
        else:
            del self.streets[street_name]

    def modify_street(self, street_name, new_segments):
        if street_name not in self.streets:
            raise Exception('`mod` specified for a street does not exist.')
        else:
            self.streets[street_name] = new_segments

    def generate_graph(self):
        def check_intersection(seg_1, seg_2):
            xdiff = (float(seg_1[0][0] - seg_1[1][0]), float(seg_2[0][0] - seg_2[1][0]))
            ydiff = (float(seg_1[0][1] - seg_1[1][1]), float(seg_2[0][1] - seg_2[1][1]))

            def cross_product(a, b):
                return a[0] * b[1] - a[1] * b[0]

            def check_point_on_seg(x_, y_, seg):
                max_x, min_x = max(seg[0][0], seg[1][0]), min(seg[0][0], seg[1][0])
                max_y, min_y = max(seg[0][1], seg[1][1]), min(seg[0][1], seg[1][1])
                return min_x <= x_ <= max_x and min_y <= y_ <= max_y

            seg_1_cross_seg_2 = cross_product(xdiff, ydiff)
            d = [cross_product(seg_1[0], seg_1[1]), cross_product(seg_2[0], seg_2[1])]
            if seg_1_cross_seg_2 == 0:
                if d[0] == d[1]:  # check if these two lines are parallel, if not, they are exactly the same line
                    seg_1_1_on_seg_2 = check_point_on_seg(seg_1[0][0], seg_1[0][1], seg_2)
                    seg_1_2_on_seg_2 = check_point_on_seg(seg_1[1][0], seg_1[1][1], seg_2)
                    seg_2_1_on_seg_1 = check_point_on_seg(seg_2[0][0], seg_2[0][1], seg_1)
                    seg_2_2_on_seg_1 = check_point_on_seg(seg_2[1][0], seg_2[1][1],seg_1)
                    # check if these two line segments have overlapped parts.
                    if seg_1_1_on_seg_2 or seg_1_2_on_seg_2 or seg_2_1_on_seg_1 or seg_2_2_on_seg_1:
                        return sorted([seg_1[0], seg_1[1], seg_2[0], seg_2[1]])
                    else:
                        return 0
                else:
                    return 0

            x = cross_product(d, xdiff) / seg_1_cross_seg_2
            y = cross_product(d, ydiff) / seg_1_cross_seg_2

            return [(x, y)] if (check_point_on_seg(x, y, seg_1) and check_point_on_seg(x, y, seg_2)) else 0

        self.__init_vars__()
        for key in self.streets.keys():
            if not self.lines:
                self.__add_lines__(key)
            else:
                for new_seg in self.__get_lines__(key):
                    for old_seg in self.lines:
                        seg_coord = check_intersection(new_seg, old_seg)
                        if seg_coord == 0:
                            continue
                        elif len(seg_coord) == 4:
                            self.__vertex_mapping__(seg_coord)
                            new_line = seg_coord
                            self.__check_two_lines_overlapped(new_line)
                        else:
                            seg_coord = seg_coord[0]
                            new_vertexes = [old_seg[0], old_seg[1], new_seg[0], new_seg[1], seg_coord]
                            self.__vertex_mapping__(new_vertexes)
                            new_line1, new_line2 = [old_seg[0], seg_coord, old_seg[1]], \
                                                   [new_seg[0], seg_coord, new_seg[1]]
                            self.__check_two_lines_overlapped(new_line1)
                            self.__check_two_lines_overlapped(new_line2)
                self.__add_lines__(key)
        for line in self.line_segmented:
            for i in range(len(line) - 1):
                new_edge = [min(self.v[line[i]], self.v[line[i + 1]]), max(self.v[line[i]], self.v[line[i + 1]])]
                if new_edge not in self.e and new_edge[0] != new_edge[1]:
                    self.e.append(new_edge)
        self.__generate_output__()
        # self.old_v = self.v.copy()
        self.old_v = []


def command_parser(line):
    street_name = None
    seg = None
    keys = line.split('"')
    if len(keys) > 1 and keys[0][-1] != ' ':
        raise Exception('Need space between command and street name.')
    else:
        command = keys[0].strip()
    if len(keys) >= 2:
        street_name = keys[1].lower()
        for char in street_name:
            if (ord(char) == 32 or 65 <= ord(char) <= 90 or 97 <= ord(char) <= 122) and street_name[0]!=" ":
                continue
            else:
                raise Exception('Bad characters in street name: ' + char)
    if command == 'gg' and len(keys)>1:
        raise Exception('Unknown command format for: ' + command)
    if len(keys) == 3 and keys[2].strip():
        if command == 'rm' and keys[2].strip():
            raise Exception('Unknown command format for: ' + command)
        if keys[2][0] != ' ':
            raise Exception('Need space between street name and coordinates.')
        seg = []
        if len(keys[2].split(')(',1))>1:
            raise Exception('Missing Space between brackets')
        coordinates = keys[2].strip()
        f=0
        for i in range(0,len(coordinates)):
            if (coordinates[i]=='('and coordinates[i+1]==' ') or (coordinates[i]==' 'and coordinates[i+1]==')')or(coordinates[i]==','and coordinates[i+1]==' ') or (coordinates[i]==' 'and coordinates[i+1]==','):
                f=1
                raise Exception('Whitespaces not allowed within () of coordinates.')
            if f==1:
                continue

        if len(coordinates.split('(')) != len(coordinates.split(')')):
            raise Exception('Incorrect coordinates format.')
        coordinates_num = Counter(coordinates)['(']
        coordinates = coordinates.replace('(', ' ')
        coordinates = coordinates.replace(')', ' ')
        coordinates = coordinates.replace(',', ' ')
        coordinates = coordinates.strip()
        segments = coordinates.split()
        if len(segments) % 2 != 0 or (len(segments) / 2) != coordinates_num:
            raise Exception('Incorrect coordinates format.')
        if len(segments) <= 2:
            raise Exception('Not enough coordinates, require at least two.')
        for i in range(0, len(segments), 2):
            try:
                if '+' in segments[i].strip() or '+' in segments[i + 1].strip():
                    raise Exception('+ sign is not acceptable')
                seg.append((int(segments[i].strip()), int(segments[i + 1].strip())))
            except Exception as e:
                raise e
    return command, street_name, seg


def main():
    # YOUR MAIN CODE GOES HERE
    # sample code to read from stdin.
    # make sure to remove all spurious print statements as required
    # by the assignment

    street_map = StreetMap()

    while True:
        line = sys.stdin.readline().strip('\n')
        if line.rstrip() == "":
            break
        try:
            command, street_name, segments = command_parser(line)
            if command == 'gg':
                street_map.generate_graph()
                street_map.start_idx = 1
            elif command == 'add' and street_name and segments:
                street_map.add_street(street_name, segments)
            elif command == 'mod' and street_name and segments:
                street_map.modify_street(street_name, segments)
            elif command == 'rm' and street_name:
                street_map.remove_street(street_name)
            else:
                raise Exception('Unknown command: ' + command)
        except Exception as e:
            print('Error: ' + str(e), file=sys.stderr)
    # print("Finished reading input")
    # return exit code 0 on successful termination
    sys.exit(0)

if __name__ == "__main__":
    main()
