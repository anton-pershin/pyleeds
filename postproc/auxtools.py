import numpy as np

class NamedAttributesContainer(object):
    def __init__(self, elements, elements_names):
        self.elements = list(elements)
        self.elements_names = list(elements_names)

    def set_elements_names(self, elements_names):
        if len(elements_names) != len(self.elements):
            raise DimensionsDoNotMatch('Number of elements and number of elements names do not match')
        self.elements_names = list(elements_names)
        for i in range(len(self.elements)):
            setattr(self, self.elements_names[i], self.elements[i])

    def make_subcontainer(self, elem_indexes):
        subcontainer_elements = []
        subcontainer_elements_names = []
        for i in range(len(self.elements)):
            if i in elem_indexes:
                subcontainer_elements.append(self.elements[i])
                subcontainer_elements_names.append(self.elements_names[i])

        subcontainer = NamedAttributesContainer(subcontainer_elements, subcontainer_elements_names)
        subcontainer.set_elements_names(subcontainer_elements_names)
        return subcontainer

    def change_order(self, elem_indexes):
        if len(self.elements) != 1: # exception for 1d 
            if len(elem_indexes) != len(self.elements):
                raise DimensionsDoNotMatch('Number of indexes with new order and number of elements do not match')
            self.elements[:] = [self.elements[i] for i in elem_indexes]
            if self.elements_names != []:
                self.elements_names[:] = [self.elements_names[i] for i in elem_indexes]

    def update_attributed_elements(self):
        for i in range(len(self.elements_names)):
            setattr(self, self.elements_names[i], self.elements[i])

    def convert_names_to_indexes_if_necessary(self, names):
        indexes = []
        if isinstance(names[0], basestring):
            indexes = []
            for name in names:
                indexes.append(self.elements_names.index(name))
        else:
            indexes = names

        return indexes

class DimensionsDoNotMatch(Exception):
    pass

class LabeledValue(object):
    def __init__(self, val, label):
        self.val = val
        self.label = label

    def __str__(self):
        return str(self.val)

class LabeledList(object):
    def __init__(self):
        self.values = []
        self.label = None

    def append(self, labeled_value):
        if self.label is None:
            self.label = labeled_value.label
        elif self.label != labeled_value.label:
            raise LabelsDoNotMatch('List label and appended value label do not match')

        self.values.append(labeled_value.val)

class LabelsDoNotMatch(Exception):
    pass