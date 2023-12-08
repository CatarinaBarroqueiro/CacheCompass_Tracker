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

    async save( discovery: { idDiscovery:number ,idBox: string, idUser: string, discTime: string, authorized: boolean}) {
        return await this.discoveryRepository.save(discovery);
    }

    async remove(idDiscovery: string) {
        return await this.discoveryRepository.delete(idDiscovery);
    }

    async findBox(idBoxS: string) {
        return await this.discoveryRepository.find({
            loadRelationIds: true,
            where: { idBox: idBoxS }
        });
    }
}
