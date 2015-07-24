# You first have to set the shell environement for medcoupling
# (see medcoupling/env.sh)

all:
	python simpledemo.py

clean:
	rm merge.med *~
