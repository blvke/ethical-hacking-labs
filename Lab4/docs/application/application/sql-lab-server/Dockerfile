FROM php:apache

RUN docker-php-source extract 
COPY ./app /var/www/html

RUN chown -R www-data:www-data /var/www
