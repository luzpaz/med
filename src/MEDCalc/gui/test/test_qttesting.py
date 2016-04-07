# Copyright (C) 2011-2016  CEA/DEN, EDF R&D
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
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
# Author: A. Bruneton (CEA)

import unittest, os
from posixpath import basename

class MEDGUITest(unittest.TestCase):
  def __init__(self, methodName='runTest'):
    unittest.TestCase.__init__(self, methodName=methodName)
    self._tmpDir = ""
    self._removeDir = True
    
  def setUp(self):
    import tempfile
    self._tmpDir = tempfile.mkdtemp(prefix="med_gui_tests_")
    self._removeDir = True  # reset for each new test in the TestCase

  def tearDown(self):
    import shutil
    unittest.TestCase.tearDown(self)
#    if self._removeDir:
#      shutil.rmtree(self._tmpDir, False)
    
  def getTestName(self):
    """ Return name of the test being currently executed. """
    return self.id().split(".")[-1]

  def launchSalomeWithScript(self, scriptname, baseline):
    """ TODO: review this - what is the nicest way to launch SALOME GUI from a Python script? """
    import shutil, subprocess
    from medcalc_testutils import GetScriptDir
    # TODO: review this!
    salomeCommand = os.path.join(os.environ.get("KERNEL_ROOT_DIR", ""), "bin", "salome", "runSalome.py")
    pth = os.path.join(GetScriptDir(), scriptname)
    # Remove a potentially already present image file from the tmp directory:
    gen_image = os.path.join("/tmp", baseline)
    try:
      shutil.rmtree(gen_image)
    except OSError:
      pass
    # Launch SALOME with the test script:
    status = subprocess.call([salomeCommand, pth])
    if status:
      raise Exception("SALOME exited abnormally for this test!")
    try:
      # Move generated image to the temporary test directory - ideally test should produce image there directly ...
      shutil.move(gen_image, self._tmpDir)
    except IOError:
      raise Exception("Test script didn't produce expected image '%s'!" % gen_image)

  def compareSnapshot(self, basename):
    import filecmp
    from medcalc_testutils import GetBaselineDir
    
    base_pth = os.path.join(GetBaselineDir(), basename)
    gen_path = os.path.join(self._tmpDir, basename)
    print base_pth, gen_path
    try:
      ret = filecmp.cmp(base_pth, gen_path, shallow=False)
    except OSError:
      ret = False
    if not ret:
      # Keep file if comparison fails
      self._removeDir = False
      self.assertTrue(ret, "[%s] -- Failed screenshot equality, or unable to open baseline file - directory is kept alive: %s" % (self.getTestName(), self._tmpDir))
    return ret

  ##
  ## Now the tests themselves
  ##
  
  def testScalarMap(self):
    baseline = "test_scalarmap.png"
    self.launchSalomeWithScript("test_scalarmap.py", baseline)
    self.compareSnapshot(baseline)
    
if __name__ == "__main__":
  suite = unittest.TestSuite()
  suite.addTest(MEDGUITest('testScalarMap'))
#  suite.addTest(MEDGUITest('testIsoContour'))
  unittest.TextTestRunner().run(suite)

