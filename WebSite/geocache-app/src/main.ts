import { ValidationPipe } from '@nestjs/common';
import { ConfigService } from '@nestjs/config';
import { NestFactory } from '@nestjs/core';
import { NestExpressApplication } from '@nestjs/platform-express';
import { AppModule } from './app.module';


const CORS_OPTIONS = {

  origin: true,
  allowedHeaders: [
    'Origin',
    'X-Requested-With',
    'Accept',
    'Content-Type',
    'Authorization'
  ],

  methods: ['GET', 'PUT', 'OPTIONS', 'POST', 'DELETE'],
};

async function bootstrap() {
  const app = await NestFactory.create(AppModule);

  app.enableCors(CORS_OPTIONS);

  app.useGlobalPipes(new ValidationPipe({ whitelist: true, transform: true }));

  await app.listen(3000, '0.0.0.0', () => {
    console.log('[WEB]');
  });
}

bootstrap();