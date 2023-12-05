import { Controller, Get, HttpStatus, Query, Post, Body, Req, Headers, Delete } from '@nestjs/common';
import { BoxService } from './box.service';
import * as moment from 'moment';
import { AppService } from '../../app.service';
import { Request } from 'express';

@Controller('box')
export class BoxController {
    res;
    constructor(private readonly boxService: BoxService,private readonly appService: AppService) { }

    @Get()
    async getBoxes(@Req() request: Request, @Headers() headers: { authorization: string }) {
      
        let api = {
            op: 'Get Boxes',
            date: moment().toString(),
            request: request,
            result: null,
            validation: null
        };

        try {
            api.result = await this.boxService.findAll();
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
    async postboxes(@Body()box: { idBox: string, local: string}) {
        try {
            return await this.boxService.save(box);
        } catch (e) {
            return "Post Boxes failed";
        }
    
    }

    @Delete()
    async deleteboxs(@Query('idBox') idBox: string) {
        try {
            return await this.boxService.remove(idBox);
        } catch (e) {
            return "Delete Boxes failed";
        }

    }
}
