import { Controller, Get, HttpStatus, Query, Post, Body, Req, Headers, Delete } from '@nestjs/common';
import { DiscoveryService } from './discovery.service';
import * as moment from 'moment';
import { AppService } from '../../app.service';
import { Request } from 'express';

@Controller('discovery')
export class DiscoveryController {
    res;
    constructor(private readonly discService: DiscoveryService,private readonly appService: AppService) { }


    @Get()
    async getDiscovery(@Req() request: Request, @Headers() headers: { authorization: string }) {
      
        let api = {
            op: 'Get Discoveries',
            date: moment().toString(),
            request: request,
            result: null,
            validation: null
        };

        try {
            api.result = await this.discService.findAll();
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

    @Get('/idDis')
    async getIdDiscovery(@Query('idDis') idDis : number) {
        try{
           return await this.discService.findDiscovery(idDis) 
        }catch(e){
            console.log(e)
            return "Get Discovery failed";
        }   
    }

    @Get('/user')
    async getUserDisc(@Query('user') idUser : number) {
        try{
           return await this.discService.findDiscUser(idUser) 
        }catch(e){
            console.log(e)
            return "Get User Discovery failed";
        }   
    }

    @Get('/box')
    async getBoxDisc(@Query('box') idBox : number) {
        try{
           return await this.discService.findDiscBox(idBox) 
        }catch(e){
            console.log(e)
            return "Get Box Discovery failed";
        }   
    }

    @Get('/auth')
    async getAuth(@Query('auth') auth : boolean) {
        try{
           return await this.discService.findDiscAuth(auth) 
        }catch(e){
            console.log(e)
            return "Get Authorized/Non-Authorized Discovery failed";
        }   
    }


    @Post()
    async postDiscovery(@Body()discovery: { idDiscovery:number ,idBox: number, idUser: number, discTime: string, authorized: boolean}) {
        try {
            return await this.discService.save(discovery);
        } catch (e) {
            return "Post Discovery failed";
        }
    
    }

    @Delete()
    async deleteDiscovery(@Query('idDiscovery') idDiscovery: string) {
        try {
            return await this.discService.remove(idDiscovery);
        } catch (e) {
            return "Delete Discovery failed";
        }

    }
}
