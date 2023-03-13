##########################################################################
# basf2 (Belle II Analysis Software Framework)                           #
# Author: The Belle II Collaboration                                     #
#                                                                        #
# See git log for contributors and copyright holders.                    #
# This file is licensed under LGPL-3.0, see LICENSE.md.                  #
##########################################################################

import modularAnalysis as ma
import rawdata as raw
import reconstruction as re
from daqdqm.commondqm import add_common_dqm
import basf2 as b2
import mdst
import logging
import sys

iteration = int(sys.argv[1])
events = 1000

b2.set_log_level(b2.LogLevel.ERROR)
b2.logging.add_file("zzzlogFile.txt")

#path = "/group/belle2/dataprod/Data/PromptSkim/e0012/biidp2571/r05500/skim/mumu_2trk/raw/sub00/"
#path = "/group/belle2/dataprod/Data/PromptSkim/e0012/biidp2571/r05466/skim/mumu_2trk/raw/sub00/"
path = "/home/belle2/dharper/muondata/"
files = path + "*.root"

main = b2.create_path()

# The first tag is need for running on current development release
b2.conditions.globaltags = ['data_reprocessing_proc13', 'online',
                            'dp_recon_release6_patch',
                            'ecl_release06_forOnline']

b2.conditions.prepend_globaltag('release-06-00-07')

# This line is currently set up to get right to the SEGFAULT (3 events in),
# but substituting the commented part yields the original functionality
main.add_module("RootInput", inputFileNames=files, skipNEvents=10_449)#iteration * events)
main.add_module("HistoManager", histoFileName=f"test{iteration}.root")
main.add_module('Gearbox')
main.add_module('Geometry')
raw.add_unpackers(main)
re.add_reconstruction(main, add_muid_hits=True, pruneTracks=True)


klmcdc = b2.register_module('KLMCDCTrack')
main.add_module(klmcdc)

#mdst.add_mdst_output(main, mc=False, filename='mdstMuonData.root')

main.add_module('Progress')
b2.print_path(main)


# Process given number of events
b2.process(main, max_event=events)
