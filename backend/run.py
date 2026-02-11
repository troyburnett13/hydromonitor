#! /usr/bin/env python
from app import app, Config, Mqtt



if __name__ == "__main__":   

    # RUN FLASK APP
    app.run(debug=Config.FLASK_DEBUG, host=Config.FLASK_RUN_HOST, port=Config.FLASK_RUN_PORT)
    
