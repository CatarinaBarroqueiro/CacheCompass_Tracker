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


    @Post()
    async postDiscovery(@Body()discovery: { idDiscovery:number ,idBox: string, idUser: string, discTime: string, authorized: boolean}) {
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
