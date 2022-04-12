python -m venv env/ &>/dev/null
source env/bin/activate &>/dev/null
pip install termcolor &>/dev/null
pip install pyyaml &>/dev/null

python tests_suite.py --binary ../builddir/42sh

deactivate
