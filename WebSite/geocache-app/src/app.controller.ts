import { Controller, Get, HttpStatus, Query, Post, Body, Req, Headers } from '@nestjs/common';
import { AppService } from './app.service';
import * as moment from 'moment';
import { Request } from 'express';



@Controller()
export class AppController {
  constructor(private readonly appService: AppService) {
  }

  @Get('/')
  getAPI(@Req() request: Request): string {
    return 'Geocache Backend is working!';
  }
}
