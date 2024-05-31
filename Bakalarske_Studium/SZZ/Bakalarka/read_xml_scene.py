"""Module for parsing XML with scene"""

__autor__ = "Peter Ďurica"
__contact__ = "xduric05@vutbr.cz"
__version__ = "1.0"

import xml.etree.ElementTree as ET


def read_xml_scene(xml_file):
    Objects = []

    try:
        tree = ET.parse(xml_file)
    except:
        print('Error: Could not parse XML file')
        return None
    scene = tree.getroot()
    for child in scene:
        if child.tag == 'shape':

            shape = {}

            for key in child.attrib:
                shape[key] = child.attrib[key]

            for element in child:
                if element.tag == "string" or element.tag == "transform":    
                    element_dict = {}

                    for key in element.attrib:
                        element_dict[key] = element.attrib[key]

                    if element.tag == "transform":
                        transformations = []

                        for transformation in element:
                            trans_dict = {"type": transformation.tag}
                            
                            for key in transformation.attrib:
                                trans_dict[key] = transformation.attrib[key]
                            
                            transformations.append(trans_dict)
                    
                        element_dict[element.tag] = transformations

                    shape[element.tag] = element_dict

            Objects.append(shape)

    return Objects

def CheckXml(xml_file):
    try:
        ET.parse(xml_file)
    except:
        return False
    else:
        return True
