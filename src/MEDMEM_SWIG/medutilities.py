# -*- coding: iso-8859-1 -*-
# --
# Copyright (C) 2009-2012  CEA/DEN, EDF R&D, OPEN CASCADE
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#
# Author : Erwan ADAM (CEA)
# --

from libMEDMEM_Swig import *

def my_remove(f):
    from os import remove
    try:
        remove(f)
    except OSError:
        pass
    return

def sauv2med(*argv):
    argv = list(argv)
    # argv = argv[1:]
    for arg in argv:
        convert(arg, "GIBI", "MED")
        pass
    return

def med2sauv(*argv):
    argv = list(argv)
    # argv = argv[1:]
    format = 1
    for arg in argv[:]:
        if arg.find('--format') == 0:
            argv.remove(arg)
            try:
                value = arg.split("=")[1]
            except IndexError:
                usage(1)
                pass
            try:
                value = int(value)
            except ValueError:
                usage(1)
                pass
            format = value
            pass
        pass
    for arg in argv:
        convert(arg, "MED", "GIBI", format)
        pass
    return

def convert(file_in, driver_in, driver_out, format=1, file_out=None):
    #
    print file_in
    #
    if file_out is None:
        file_out = file_in
##        if file_out.find('.') != -1:
##            suffix = file_in.split('.')[-1]
##            if driver_in == "GIBI":
##                test = "sauv"
##            else:
##                test = "med"
##                pass
##            if len(suffix) >= len(test):
##                suffix = suffix[:len(test)]
##                suffix = suffix.lower()
##                if suffix == test:
##                    file_out = '.'.join(file_in.split('.')[:-1])
##                    pass
##                pass
##            pass
        if driver_out == "GIBI":
            file_out += ".sauv"
        elif driver_out == "MED":
            file_out += ".med"
        else:
            msg = "Driver out %s is unknown"%(driver_out)
            raise NotImplementedError(msg)
        pass
    print file_out
    #
    meshes = []
    fields = []
    if driver_in == "GIBI":
        driver = GIBI_MED_RDONLY_DRIVER(file_in)
        fields = driver.read()
        mesh = driver.getMesh()
        if mesh:
            meshes.append( mesh )
    elif driver_in == "MED":
        med = MEDFILEBROWSER(file_in)
        for mesh_name in med.getMeshNames():
            if med.isStructuredMesh( mesh_name ):
                mesh = GRID( MED_DRIVER, file_in, mesh_name )
            else:
                mesh = MESH( MED_DRIVER, file_in, mesh_name )
            meshes.append( mesh )
        for field_name in med.getFieldNames():
            mesh_name = med.getMeshName( field_name )
            mesh = 0
            for m in meshes:
                if m.getName() == mesh_name:
                    mesh = m; break
            for dtit in med.getFieldIteration( field_name ):
                if med.getFieldType( field_name ) == MED_REEL64:
                    field = FIELDDOUBLE(MED_DRIVER, file_in, field_name, dtit.dt, dtit.it, mesh )
                else:
                    field = FIELDINT(MED_DRIVER, file_in, field_name, dtit.dt, dtit.it, mesh )
                fields.append( field )
    else:
        msg = "Driver in %s is unknown"%(driver_in)
        raise NotImplementedError(msg)
    #
    my_remove(file_out)
    #
    if driver_out == "GIBI":
        mesh = meshes[0]
        mesh_dim = mesh.getSpaceDimension()
        if format == 0:
            file_out = file_out+'__format__'
            my_remove(file_out)
            pass
        if fields:
            driver = GIBI_MED_WRONLY_DRIVER(file_out, fields, mesh)
        else:
            driver = GIBI_MESH_WRONLY_DRIVER(file_out, mesh)
        driver.open()
        driver.write()
        driver.close()
        #
        if mesh_dim >= 3:
            from sys import platform
            if platform in ["win32"]:
                f = open(file_out)
                content = f.read()
                f.close()
                content = content.replace("IFOUR  -1", "IFOUR   2")
                content = content.replace("IFOMOD  -1", "IFOMOD   2")
                f = open(file_out, "w")
                f.write(content)
                f.close()
            else:
                cmd  = "sed"
                cmd += ' -e "s/IFOUR  -1/IFOUR   2/g"'
                cmd += ' -e "s/IFOMOD  -1/IFOMOD   2/g"'
                # cmd += ' -e "s/IECHO   1/IECHO   0/g"'
                cmd += ' %s > .dummy'%(file_out)
                cmd += ' && '
                cmd += ' mv -f .dummy %s'%(file_out)
                from os import system
                system(cmd)
                pass
            pass
        #
        if format == 0:
            from castemlauncher import CastemLauncher
            dgibi_stream  = "\n"
            dgibi_stream += "OPTI REST FORMAT '%s' ;\n"%(file_out)
            dgibi_stream += "REST FORMAT;\n"
            file_out = file_out.replace('__format__', '')
            dgibi_stream += "OPTI SAUV '%s' ;\n"%(file_out)
            dgibi_stream += "SAUV ;\n"
            cl = CastemLauncher(dgibi_stream)
            cl.addTmpFiles(file_out+'__format__', "UTILNOTI", "UTILPROC")
            cl.run()
            pass
        return
    #
    for mesh in meshes:
        mesh.write(MED_DRIVER, file_out)
    for field in fields:
        typedField = field.castToTypedField();
        typedField.write(MED_DRIVER, file_out)
    #
    return

def avs2med_one_file(file_in, file_out, mesh_name, field_name):
    """
    Convert an ucd avs inp file into a med file
    inp Specifications can be found at :
    http://people.scs.fsu.edu/~burkardt/data/ucd/ucd.html
    http://help.avs.com/Express/doc/help/reference/dvmac/UCD_Form.htm
    
    """
    my_remove(file_out)
    #
    meshing = MESHING()
    meshing.setName(mesh_name)
    #
    f = open(file_in)
    lines = f.readlines()
    f.close()
    nb_lines = len(lines)
    # ----
    # Skip the comments
    # ----
    while 1:
        l = lines[0]
        if l[0] != "#":
            break
        lines = lines[1:]
        pass
    # ----
    headers = [ int(i) for i in lines[0].split() ]
    lines = lines[1:]
    number_of_nodes = headers[0]
    number_of_cells = headers[1]
    number_of_nodes_data = headers[2]
    number_of_cells_data = headers[3]
    number_of_whole_data = headers[3]
    # ----------
    # Nodes
    # ----------
    nodes = lines[:number_of_nodes]
    lines = lines[number_of_nodes:]
    nodes = [ " ".join(l.split()[1:]) for l in nodes ]
    nodes = " ".join(nodes)
    nodes = [ float(v) for v in nodes.split() ]
    # --------
    # Space dimension
    # --------
    nodes_min = min(nodes)
    nodes_max = max(nodes)
    epsilon = 1.0e-5 * (nodes_max - nodes_min)
    nodes_z = [ nodes[i] for i in range(2, 3 * number_of_nodes, 3) ]
    nodes_z_min = min(nodes_z)
    nodes_z_max = max(nodes_z)
    space_dimension = 2
    if abs(nodes_z_max) > epsilon:
        space_dimension = 3
        pass
    if abs(nodes_z_min) > epsilon:
        space_dimension = 3
        pass
    #
    if space_dimension == 2:
        l = range(0, 3 * number_of_nodes, 3) + range(1, 3 * number_of_nodes, 3)
        l.sort()
        nodes = [ nodes[i] for i in l ]
        pass
    #
    meshing.setCoordinates(space_dimension, number_of_nodes, nodes, "CARTESIAN", MED_FULL_INTERLACE)
    # ----------
    # Cells
    # ----------
    cells = lines[:number_of_cells]
    lines = lines[number_of_cells:]
    cells = [ c.split() for c in cells ]
    #
    type2connectivity = {}
    for c in cells:
        cell_id = int(c[0])
        cell_mat = int(c[1])
        cell_type = c[2]
        cell_connectivity = c[3:]
        #
        avs_type = cell_type
        if 0:
            pass
        elif avs_type == 'tri':
            entity = MED_CELL
            nb2medtype = {
                3: MED_TRIA3,
                }
        elif avs_type == 'quad':
            entity = MED_CELL
            nb2medtype = {
                4: MED_QUAD4,
                8: MED_QUAD8,
                }
        elif avs_type == 'tet':
            entity = MED_CELL
            nb2medtype = {
                4: MED_TETRA4,
                }
        elif avs_type == 'hex':
            entity = MED_CELL
            nb2medtype = {
                8: MED_HEXA8,
                }
        elif avs_type == 'pyr':
            entity = MED_CELL
            nb2medtype = {
                5: MED_PYRA5,
                }
        elif avs_type == 'prism':
            entity = MED_CELL
            nb2medtype = {
                6: MED_PENTA6,
                }
        else:
            raise Exception("unknown avs_type : %s"%(avs_type))
        #
        med_type = nb2medtype[len(cell_connectivity)]
        #
        try:
            d = type2connectivity[entity]
        except:
            type2connectivity[entity] = {}
            d = type2connectivity[entity]
            pass
        #
        try:
            l = d[med_type]
        except:
            d[med_type] = []
            l = d[med_type]
            pass
        #
        cell_connectivity = ' '.join(cell_connectivity)
        l.append(cell_connectivity)
        #
        pass
    #
    mesh_dimension = space_dimension
    #
    for entity, d in type2connectivity.items():
        meshing.setNumberOfTypes(len(d.keys()), entity)
        meshing.setTypes(d.keys(), entity)
        meshing.setNumberOfElements([len(v) for v in d.values()], entity)
        for key, value in d.items():
            connectivity = value
            connectivity = " ".join(connectivity)
            connectivity = [ int(v) for v in connectivity.split() ]
            meshing.setConnectivity(connectivity, entity, key)
            pass
        pass
    # -----------
    meshing.write(meshing.addDriver(MED_DRIVER,file_out,meshing.getName()))
    # -----------
    flag = -1
    for n in [number_of_nodes_data, number_of_cells_data]:
        flag += 1
        if n == 0:
            continue
        # -----------
        header = [ int(v) for v in lines[0].split() ]
        lines = lines[1:]
        nb_of_components = header[0]
        nb_of_values_by_component = header[1:]
        if len(nb_of_values_by_component) != nb_of_components:
            msg = "Error at line %d"%(nb_lines - len(lines))
            raise Exception(msg)
        #
        titles_by_component = []
        for i in range(nb_of_components):
            l = lines[0]
            lines = lines[1:]
            label = l.split(',')[0]
            unit = l[len(label)+1:]   # The +1 is for the comma
            label = label.strip()
            unit = unit.strip()
            labels_by_value = nb_of_values_by_component[i]*['']
            l = [label, unit, labels_by_value]
            titles_by_component.append(l)
            pass
        if nb_of_components > 1:
            if nb_of_values_by_component == nb_of_components * [1]:
                nb_of_values_by_component = [ nb_of_components ]
                nb_of_components = 1
                if flag == 0:
                    if number_of_cells_data:
                        name = field_name + "_on_nodes"
                        pass
                    pass
                else:
                    if number_of_nodes_data:
                        name = field_name + "_on_cells"
                        pass
                    pass
                titles_by_component = [ [name, "", [t[0] for t in titles_by_component]] ]
                pass
            pass
        if flag == 0:
            nb = number_of_nodes
        else:
            nb = number_of_cells
            pass
        values = lines[:nb]
        lines = lines[nb:]
        vals = []
        imin = 1
        for i in range(nb_of_components):
            imax = imin + nb_of_values_by_component[i]
            vals.append([ l.split()[imin:imax] for l in values ])
            imin = imax
            pass
        values = vals
        # ----------
        if flag == 0:
            support = SUPPORT(meshing, "support_on_nodes", MED_NODE)
        else:
            support = SUPPORT(meshing, "support_on_cells", MED_CELL)
            pass
        for i in range(nb_of_components):
            nb = nb_of_values_by_component[i]
            field = FIELDDOUBLE(support, nb)
            field.setName(titles_by_component[i][0])
            # field.setIterationNumber(-1)
            # field.setOrderNumber(-1)
            # field.setTime(0.0)
            for n in range(nb):
                name = titles_by_component[i][2][n]
                if name:
                    field.setComponentName(n+1,name)
                    pass
                pass
            # ---------------
            value = values[i]
            value = [ " ".join(l) for l in value ]
            value = " ".join(value)
            value = [ float(v) for v in value.split() ]
            field.setValue(value)
            # -----
            field.write(field.addDriver(MED_DRIVER,file_out,field.getName()))
            # -------
            pass
        #
        pass
    return

def avs2med(*argv):
    argv = list(argv)
    #
    mesh_name = "mesh"
    field_name = "field"
    #
    for arg in argv[:]:
        if ( arg.find('--mesh_name') == 0 ) or ( arg.find('--mesh-name') == 0 ) :
            argv.remove(arg)
            try:
                mesh_name = arg.split("=")[1]
            except IndexError:
                usage(1)
                pass
            pass
        if ( arg.find('--field_name') == 0 ) or ( arg.find('--field-name') == 0 ) :
            argv.remove(arg)
            try:
                field_name = arg.split("=")[1]
            except IndexError:
                usage(1)
                pass
            pass
        pass
    #
    for arg in argv:
        avs2med_one_file(arg, arg + ".med", mesh_name, field_name)
        pass
    #
    return

