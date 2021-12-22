### Setting up the environment

```bash
git clone git@github.com:antonellopellecchia/testbeam-analysis.git
cd testbeam-analysis
source env.sh
mkdir build && cd build
cmake3 ..
make
```

### Running the code

Running the unpacker:
```bash
RawToDigi raw_file.raw ferol digi.root [n_events]
```

Local reconstruction:
```bash
DigiToRechits digi.raw rechits.root [n_events]
```

Track reconstruction:
```bash
Tracking rechits.raw tracks.root [n_events]
```

Analysis: look at python scripts in the `analysis` folder.