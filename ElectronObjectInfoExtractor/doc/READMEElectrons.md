# ElectronObjectInfo

Adapted from https://github.com/cms-opendata-analyses/PhysicsObjectsInfo

## Usage Instructions

First you have to create a [VM](http://opendata.cern.ch/VM/CMS "CMS Open Data Portal") from the CMS Open Data website. 

Then follow these steps:

- Create a CMSSW environment: 

    ```
    cmsrel CMSSW_5_3_32
    ```

- Change to the CMSSW_5_3_32/src/ directory:

    ```
    cd CMSSW_5_3_32/src/
    ```

- Initialize the CMSSW environment:

  ```
  cmsenv
  ```

- Obtain the code from git (in sparse mode) and move it to the `src` area:

  ```   
  git clone git://github.com/lmarthur/ElectronObjectInfo.git
  cd ElectronObjectInfo	


- Compile everything:

  ```
  scram b
  ```

- Make a soft link to the python configuration file

```
ln -s python/electronobjectextractor_cfg.py .
```

- Replace 'file:myfile.root' with name of the file to be analyzed

- Run the CMSSW executable in the background

```
cmsRun electronobjectextractor_cfg.py > electrons.log 2>&1 &
```

- Check the development of the job:

```
tailf electrons.log
```
