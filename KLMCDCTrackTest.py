import modularAnalysis as ma
import rawdata as raw
import reconstruction as re
from daqdqm.commondqm import add_common_dqm
import basf2 as b2

#import conditions_db as c

# Set the log level to show only error and fatal messages
b2.set_log_level(b2.LogLevel.ERROR)
#b2.set_log_level(b2.LogLevel.INFO)

path = "/group/belle2/dataprod/Data/Raw/e0018/r01553/sub00/"
files = path+"physics.0018.01553.HLT8.*.root"

# Create main path
main = b2.create_path()

# useful tags for release-06
b2.conditions.globaltags = ['patch_main_release-07', 'data_reprocessing_proc13', 'online',
                            'dp_recon_release6_patch',
                            'ecl_release06_forOnline']

b2.conditions.prepend_globaltag('release-06-00-07')

main.add_module("RootInput", inputFileNames=files)
main.add_module("HistoManager", histoFileName="KLMCDC_100events_min4_(rev2).root")
main.add_module('Gearbox')
main.add_module('Geometry')
raw.add_unpackers(main)
re.add_reconstruction(main, add_muid_hits=True)

ma.fillParticleList('mu+:all', cut="", path=main)

klmcdc = b2.register_module('KLMCDCTrack')
main.add_module(klmcdc)

#main.add_module('Progress')
#b2.print_path(main)

#db = c.ConditionsDB()
#data = db.get_all_iovs("online")
#for i in data:
#    print(i.name)

# Process all events
b2.process(main, max_event=100)
print(b2.statistics)
