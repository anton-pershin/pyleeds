from __future__ import division
import numpy as np
from field import Field, Space

def get_wave_field():
    x = np.linspace(-2*np.pi, 2*np.pi, 100)
    y = np.linspace(-2*np.pi, 2*np.pi, 100)
    X, Y = np.meshgrid(x, y, indexing='ij')
    space = Space([x, y])
    space.set_elements_names(['x', 'y'])
    wave_field = Field([np.sin(X) * np.exp(Y / 10)], space)
    wave_field.set_elements_names(['u'])
    return wave_field

def get_time_dependent_wave_fields():
    x = np.linspace(-2*np.pi, 2*np.pi, 100)
    y = np.linspace(-2*np.pi, 2*np.pi, 100)
    X, Y = np.meshgrid(x, y, indexing='ij')
    space = Space([x, y])
    space.set_elements_names(['x', 'y'])

    y_coeffs = np.linspace(1, 10, 100) 
    fields = []
    for y_coeff in y_coeffs:
        u = np.sin(X) * np.exp(Y / y_coeff)
        u = u / np.amax(u)
        wave_field = Field([u], space)
        wave_field.set_elements_names(['u'])
        fields.append(wave_field)
    return fields

def get_time_dependent_circular_flow_fields():
    x = np.linspace(-5, 5, 100)
    y = np.linspace(-2, 2, 100)
    a = 1
    U_inf = 10
    K = 0
    X, Y = np.meshgrid(x, y, indexing='ij')
    R_sq = X**2 + Y**2
    space = Space([x, y])
    space.set_elements_names(['x', 'y'])

    K_coeffs = np.linspace(0, 2 * U_inf * a, 100) 
    fields = []
    for K in K_coeffs:
        u = U_inf * (1 - (a**2 * R_sq - 2 * a**2 * Y**2) / R_sq**2) - K * a * Y / R_sq
        v = - 2 * U_inf * a**2 * X * Y / R_sq**2 + K * a * X / R_sq
        for xi in range(len(x)):
            for yi in range(len(y)):
                if x[xi]**2 + y[yi]**2 < a**2:
                    u[xi, yi] = 0.
                    v[xi, yi] = 0.

        circ_field = Field([u, v], space)
        circ_field.set_elements_names(['u', 'v'])
        fields.append(circ_field)
    return fields