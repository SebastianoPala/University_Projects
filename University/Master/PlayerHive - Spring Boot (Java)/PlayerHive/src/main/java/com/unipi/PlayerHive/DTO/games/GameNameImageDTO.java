package com.unipi.PlayerHive.DTO.games;

import lombok.AllArgsConstructor;
import lombok.Getter;
import org.springframework.data.mongodb.core.mapping.Field;

@Getter
@AllArgsConstructor
public class GameNameImageDTO {

    @Field("name")
    private String gameName;

    @Field("image")
    private String gameImage;
}
