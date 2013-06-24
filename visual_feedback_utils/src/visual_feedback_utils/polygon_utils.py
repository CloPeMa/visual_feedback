#!/usr/bin/env python

"""
"""

PACKAGE_NAME = "visual_feedback_utils"

# Imports
import roslib; roslib.load_manifest(PACKAGE_NAME)
import math

__all__ = ["polygon_test", "polygon_sector"]


def polygon_test(poly, pt):
    """
    Test whether point is in the polygon.

    :param poly: List of points that defines a polygon
    :param pt:   Point to test
    :returns:    True if point is in the polygon, false othrewise
    """
    x = pt[0]
    y = pt[1]

    # check if point is a vertex
    if (x,y) in poly:
        return True

    # check if point is on a boundary
    for i in range(len(poly)):
        p1 = None
        p2 = None
        if i == 0:
            p1 = poly[0]
            p2 = poly[1]
        else:
            p1 = poly[i - 1]
            p2 = poly[i]
        if p1[1] == p2[1] and p1[1] == y and x > min(p1[0], p2[0]) and x < max(p1[0], p2[0]):
            return True

    n = len(poly)
    inside = False

    # Check if point is inside polygon
    p1x,p1y = poly[0]
    for i in range(n + 1):
        p2x,p2y = poly[i % n]
        if y > min(p1y,p2y):
            if y <= max(p1y,p2y):
                if x <= max(p1x,p2x):
                    if p1y != p2y:
                        xints = (y - p1y) * (p2x - p1x) / (p2y - p1y) + p1x
                    if p1x == p2x or x <= xints:
                        inside = not inside
        p1x,p1y = p2x,p2y

    return inside


def polygon_sector(poly, pt):
    """
    Computes sector that remains after cutting out polygon section out of
    a circle. The polygon section is given by the list of points representing
    polygon and a point that is part of this lists, representing the section.

    This function returns a mean angle and max deviation from that angle.

    :param poly: List of points representing polygon
    :param pt:   Point representing the corner of the section
    :returns:    (mean angle, max deviation)
    """
    i = poly.index(pt)
    pt1 = poly[(i + 1) % len(poly)]
    pt2 = poly[(i - 1) % len(poly)]

    # Compute vectors that holds the angle
    v1 = tsub(pt1, pt)
    v2 = tsub(pt2, pt)

    # Check convexity of the segment
    v3 = tsub(pt1, pt2)
    pt3 = tadd(tmul(v3,0.5),pt2)
    if polygon_test(poly, pt3):
        convex = True
    else:
        convex = False

    # Compute base angle
    theta1 = math.atan2(v1[1], v1[0])
    theta2 = math.atan2(v2[1], v2[0])

    gamma = adiff(theta1, theta2)
    alpha = nangle(amean([theta1, theta2]), signed = True)

    if convex and gamma < math.pi:
        gamma = 2 * math.pi - gamma
        alpha = nangle(nangle(alpha, signed = False) + math.pi, signed = True)
    elif not convex and gamma > math.pi:
        gamma = 2 * math.pi - gamma
        alpha = nangle(nangle(alpha, signed = False) + math.pi, signed = True)

    beta = gamma / 2
    return (alpha, beta)

###############################################################################
# Helper Functions                                                            #
###############################################################################


def tadd(a, b):
    return tuple(map(lambda x, y: x + y, a, b))


def tsub(a, b):
    return tuple(map(lambda x, y: x - y, a, b))


def tmul(a, s):
    return tuple(map(lambda x: x * s, a))


def rad2deg(r):
    return (r / (2 * math.pi)) * 360


def deg2rad(d):
    return (d / 360) * (2 * math.pi)


def adiff(a, b):
    return math.pi - abs(abs(a - b) - math.pi)


def nangle(a, signed = True):
    """
    Normalize angle
    """
    if signed:
        if a > math.pi:
            a = nangle(a - 2 * math.pi, signed)
        elif a < -math.pi:
            a = nangle(a + 2 * math.pi, signed)
    else:
        if a > 2 * math.pi:
            a = nangle(a - 2 * math.pi, signed)
        elif a < 0:
            a = nangle(a + 2 * math.pi, signed)
    return a


def amean(angles):
    ms = sum(map(math.sin, angles)) / len(angles)
    mc = sum(map(math.cos, angles)) / len(angles)
    return math.atan2(ms,mc)

###############################################################################
# Main for testing purposes                                                   #
###############################################################################

poly = [(2,1), (4,1), (4,4), (2,4), (1,5), (1,3)]

if __name__ == '__main__':
    for p in poly:
        angle, dev = polygon_sector(poly, p)
        print rad2deg(angle), rad2deg(dev)
