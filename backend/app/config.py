
from os import  environ
from os.path import  abspath, dirname 
from dotenv import load_dotenv

load_dotenv()  # load environment variables from .env if it exists.
basedir = abspath(dirname(__file__))

class Config(object):
    """Base Config Object"""
    

    FLASK_DEBUG                             = eval(environ.get('DEBUG','False'))
    SECRET_KEY                              = environ.get('SECRET_KEY', 'Som3$ec5etK*y')
    UPLOADS_FOLDER                          = environ.get('UPLOADS_FOLDER') 
    IMAGE_FOLDER                            = environ.get('IMAGE_FOLDER') 

    ENV                                     = environ.get('FLASK_ENV') 
    FLASK_RUN_PORT                          = int(environ.get('FLASK_RUN_PORT', 5000))
    FLASK_RUN_HOST                          = environ.get('FLASK_RUN_HOST', '0.0.0.0') 

    # MONGODB VARIABLES
    DB_USERNAME                             = environ.get('DB_USERNAME','') 
    DB_PASSWORD                             = environ.get('DB_PASSWORD','') 
    DB_SERVER                               = environ.get('DB_SERVER','www.yanacreations.com') 
    DB_PORT                                 = int(environ.get('DB_PORT', 27017))

    # Optional TLS flag for MongoDB
    DB_TLS                                  = eval(environ.get('DB_TLS','False'))

    PROPAGATE_EXCEPTIONS                    = False
 
 
