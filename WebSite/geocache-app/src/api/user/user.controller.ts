import { Controller, Get, HttpStatus, Query, Post, Body, Req, Headers, Delete } from '@nestjs/common';
import { UserService } from './user.service';
import * as moment from 'moment';
import { AppService } from '../../app.service';
import { Request } from 'express';

@Controller('user')
export class UserController {
    res;
    constructor(private readonly usersservice: UserService,private readonly appService: AppService) { }

    @Get()
    async getusers(@Req() request: Request, @Headers() headers: { authorization: string }) {
      
        let api = {
            op: 'Get Users',
            date: moment().toString(),
            request: request,
            result: null,
            validation: null
        };

        try {
            api.result = await this.usersservice.findAll();
            if (api.result !== null) {
                this.res = this.appService.handleResponse(true, 'Done! ✔️', HttpStatus.OK, api);
            } else {
                this.res = this.appService.handleResponse(false, 'Server error! ❌️', HttpStatus.INTERNAL_SERVER_ERROR, api);
            }
        } catch (error) {
            api.validation = null;
            api.result = error;
            this.res = this.appService.handleResponse(false, 'Server error! ❌️', HttpStatus.INTERNAL_SERVER_ERROR, api);
        }
        return this.res;


    }

   

    @Post()
    async postusers(@Body() user: { idUser: string, name: string, email: string, flag:boolean, password:string, admin:boolean}) {
        try {
            return await this.usersservice.save(user);
        } catch (e) {
            return "Post Users failed";
        }
    
    }

    @Delete()
    async deleteusers(@Query('idUser') idUser: string) {
        try {
            return await this.usersservice.remove(idUser);
        } catch (e) {
            return "Delete Users failed";
        }

    }
}
