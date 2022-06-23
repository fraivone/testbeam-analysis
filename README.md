### Setting up the environment

```bash
git clone git@github.com:antonellopellecchia/testbeam-analysis.git
cd testbeam-analysis
source env.sh
mkdir build && cd build
cmake3 ..
make
```

### IntegrationStand

Running the unpacker:
```bash
./RawToDigi /afs/cern.ch/user/f/fivone/public/data/run000000_ls0001_index00*.raw bla.root
```

Latency Plot:
```bash
python3 latency_analyzer.py /afs/cern.ch/user/f/fivone/Documents/Unpacker_2022/testbeam-analysis/build/bla.root /eos/user/f/fivone/www/IntegrationStand/GE21/  --verbose -n 8000
```