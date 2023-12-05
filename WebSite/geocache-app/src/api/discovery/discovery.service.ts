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
        return await this.discoveryRepository.find(
            { loadRelationIds: true}
        );
    }

    async save( discovery: { idDiscovery:string ,id_Box: string, id_user: string, discTime: string, authorized: boolean}) {
        return await this.discoveryRepository.save(discovery);
    }

    async remove(idDiscovery: string) {
        return await this.discoveryRepository.delete(idDiscovery);
    }
}
