import { Injectable } from '@nestjs/common';
import { Discovery } from './discovery.entity';
import { InjectRepository } from '@nestjs/typeorm';
import { Repository } from 'typeorm';

@Injectable()
export class DiscoveryService {
    constructor(
        @InjectRepository(Discovery)
        private discoveryRepository: Repository<Discovery>
    ) {

    }

    async findAll() {
        return await this.discoveryRepository.manager.query('Select * from "Discovery"');
    }

    async findDiscovery(idDiscSearched: number) {
        return await this.discoveryRepository.find({
            loadRelationIds: true,
            where: { idDiscovery: idDiscSearched }
        });
    }

    async findDiscBox(idBoxSearched: number) {
        return await this.discoveryRepository.find({
            loadRelationIds: true,
            where: { box: idBoxSearched } as any,
        });
    }
    
    async findDiscUser(idUserSearched: number) {
        return await this.discoveryRepository.find({
            loadRelationIds: true,
            where: { user: idUserSearched } as any,
        });
    }

    async findDiscAuth(authSearched: boolean) {
        return await this.discoveryRepository.find({
            loadRelationIds: true,
            where: { authorized: authSearched }
        });
    }


    async save( discovery: { idDiscovery:number ,idBox: number, idUser: number, discTime: string, authorized: boolean}) {
        return await this.discoveryRepository.save(discovery);
    }

    async remove(idDiscovery: string) {
        return await this.discoveryRepository.delete(idDiscovery);
    }

}
