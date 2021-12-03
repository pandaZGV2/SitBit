default:
	export FLASK_APP=app.py FLASK_DEBUG=1 && flask run

clean:
	python3 ci_delete.py
	python3 ci_create.py